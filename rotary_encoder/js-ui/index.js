const app = require('express')()
const http = require('http').createServer(app)
const io = require('socket.io')(http)

const SerialPort = require('serialport')
const Readline = require('@serialport/parser-readline')

const path = '/dev/tty.usbmodem14101'
const port = new SerialPort(path, { baudRate: 115200 })

const parser = new Readline()
port.pipe(parser)

parser.on('data', line => {
    console.log(`> ${line}`)
    io.emit('data', line)
})

port.write('ROBOT POWER ON\n')


app.get('/', function(req, res){
 res.sendFile(__dirname + '/index.html')
})

io.on('connection', function(socket){
    console.log('Incoming connection')  
})

http.listen(3000, function(){
  console.log('listening on *:3000')
})