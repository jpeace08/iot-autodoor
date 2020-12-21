/*goi thư viện để cấu hình server*/
var express = require('express');
const { disconnect, exit } = require("process");
var app = express();

var server = require('http').Server(app);
var mysql = require('mysql');

var bodyParser =  require('body-parser');
var logger =  require ('morgan');

/*Goi thưu viện socket io*/
var io = require('socket.io')(server);

app.use(express.static('public'));
app.set('view engine', 'ejs');
app.set('views', './views');
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false }));
app.use(logger('dev'));

server.listen(3000);

let stateDoor;
let isEnable;

// kết thúc cấu hình server

// connect database:
var con = mysql.createConnection({
    host: "localhost",
    user: "root",
    password: "",
    database: "iot_autodoor"
  });
  
  con.connect(function(err) {
    if (err) throw err;
    console.log("Connected!");
  });

//Hàm kiểm tra sáng tối để ngắt() :3
const sendShut = () => {
    let date = new Date;
    let hour = date.getHours();
    let minute = date.getMinutes();
    let sencond = date.getSeconds();
    if(minute === 0 && sencond === 0){
        if(hour === 6){
            io.emit("status-change", JSON.parse('{"sts":"'+3+'"}'));
            isEnable = 3;
            
        }
        if(hour === 18){
            isEnable = 4;
            io.emit("status-change", JSON.parse('{"sts":"'+4+'"}'));
        }

        con.query(`update iot_controll set state = ${isEnable} where id = 2`, function (err, result) {
            if (err) throw err;
        });
        con.commit();
    }
}

//gọi hàm kiểm tra ở trên
setInterval(sendShut, 1000);

//khi esp kết nối đến server thì chạy vào đây
io.on('connection', (socket) => {

    //server gửi giá trị mặc định cho esp
    io.to(socket.id).emit("status-change", JSON.parse('{"sts":"'+0m+'"}'));

    socket.on('disconnect', () => {
        console.log(`${socket.id} disconnectted`);
    });

});

//khi submit tren giao diện web
app.post('/state', (req, res) => {
    //kiêm tra trạng thái cổng và lưu vào database
    if(0 === parseInt(req.body.state)){
        stateDoor=0;
    }
    else {stateDoor = 1;}
    //lưu vào database
    con.query(`update iot_controll set state = ${stateDoor} where id = 1`, function (err, result) {
        if (err) throw err;
    });
    con.commit();
    //gửi tín hiệu đóng mở cổng đến esp 
    io.emit('status-change', JSON.parse('{"state":"'+req.body.state+'"}'));
    res.send();
});

app.get('/', (req, res) => {
    
    //lấy ra trạng thái cổng và trạng thái ngắt để show lên giao diện
    con.query("select * from iot_controll", function (err, result) {
        if (err) throw err;
        
        isEnable = result[1].state;
        stateDoor = result[0].state;

    });

    res.render('index', {isEnable});
});

