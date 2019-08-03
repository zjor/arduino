const express = require('express')
const app = express()
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

app.use(express.static('static'))

app.get('/', function(req, res){
 res.sendFile(__dirname + '/index.html')
})

io.on('connection', function(socket){
    console.log('Incoming connection')  
})

http.listen(3000, function(){
  console.log('listening on *:3000')
})

// generate test data
setInterval(() => {
  const now = (new Date().getTime()) / 1000 * Math.PI
  const value = (Math.sin(now)) * Math.PI
  const value2 = (Math.cos(now)) * Math.PI
  io.emit('data', `${value}\t${value2}`)
}, 50)