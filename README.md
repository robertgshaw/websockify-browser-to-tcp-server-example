# websockify-browser-to-tcp-server-example
Example demonstrating how to connect a browser (which utilizes Websockets) to a TCP server (in this case, a C++ server using Berkeley sockets) using websockify as a proxy.

### Problem Statement
I had been working on a pet project (a multi-player version of pacman) to teach myself C++, concurrency (e.g. threads, locks, condition variables), and networking. I had built a client-server architecture (both written in C++) which utilized typical Berkeley sockets that come out of the box with Linux and C++ to communicate via TCP/IP. Given this is a real-time game server, the app was not RESTful (i.e. not using HTTP) and needed to used full duplex long lived connections.

As part of this I wanted to expand this project to make the client into a Web-based application using React for the front-end + Flask as the api for some adjacent RESTful services (e.g. landing page, creating users, etc). HOWEVER, I soon discovered that it is nontrivial to connect a TCP server to a browser! Fortunately, modern browsers have the concept of `WebSockets` which I stumbled upon that are similiar in concept to traditional Berkeley sockets - in particular, allowing full duplex communication over a single TCP connection - but are less generic (i.e. WebSockets are application level protocols over TCP - just like HTTP). In effect, You can read more about them [Here](https://en.wikipedia.org/wiki/WebSocket).

*As such, I needed to figure out how to connect my TCP server (where I had already written a lot of functionality) to a WebSocket*

### Potential Options

As I looked into how to solve this problem, there were a few things I stumbled upon. 
  1. First, is `Socket.IO` which is a common library that implements an even higher level protocol on top WebSockets. Serveral common Web Frameworks / languages appear to have           great support for `Socket.IO`, including `flask` which you can see [here](https://flask-socketio.readthedocs.io/en/latest/) and `node`. Unfortunately, I could not find a           server side implementation of `Socket.IO` in C++ (though there are some clients available). 

`https://docs.microsoft.com/en-us/windows/win32/winsock/complete-server-code?redirectedfrom=MSDN`
