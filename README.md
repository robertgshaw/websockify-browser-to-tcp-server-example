# websockify-browser-to-tcp-server-example
Example demonstrating how to connect a browser (which utilizes `Websockets`) to a `TCP` server (in this case, a `C++` server using Berkeley sockets) using `websockify` as a proxy.

### Problem Statement
I had been working on a pet project (a multi-player version of pacman) to teach myself `C++`, concurrency (e.g. threads, locks, condition variables), and networking. I had built a client-server architecture (both written in `C++`) which utilized typical Berkeley sockets that come out of the box with Linux and `C++` to communicate via `TCP/IP`. Given this is a real-time game server, the app was not RESTful (i.e. not using HTTP) and needed to use full duplex long-lived connections and the server was stateful.

As part of this, I wanted to expand this project to make the client into a Web-based application using React for the front-end + Flask as the api for some adjacent RESTful services (e.g. landing page, creating users, etc). HOWEVER, I soon discovered that it is nontrivial to connect a `TCP` server to a browser! Fortunately, modern browsers have the concept of `WebSockets` which I stumbled upon that are similiar in concept to traditional Berkeley sockets - in particular, allowing full duplex communication over a single TCP connection - but are less generic (i.e. WebSockets are application level protocols over `TCP` - just like `HTTP`). You can read more about them [Here](https://en.wikipedia.org/wiki/WebSocket).

***As such, I needed to figure out how to connect my C++ game server using Berkeley sockets with TCP to a Browser using WebSockets***

### Potential Options Considered

As I looked into how to solve this problem, there were a few things I stumbled upon. 

**1.`Socket.IO`** 
- Socket.IO is a common library that implements an even higher level protocol on top WebSockets. 
- Serveral common Web Frameworks / languages appear to hav good support for `Socket.IO`, including `flask` which you can see [here](https://flask-socketio.readthedocs.io/en/latest/) and `node`. Unfortunately, I could not find a server side implementation of `Socket.IO` in C++ (though there are some clients available). So this was a no go, but might be a good option if you have a `python` or `node` application.
 
**2.`Refactoring my C++ server to directly use `WebSockets`** 
- I could have used the `WebSocket` API directly in my C++, which would have required me to refactor my existing codebase to use the higher level protocol. This would have required either implementing the `WebSocket` protocol by hand or using a 3rd party implementation of `WebSockets`. I did not want to reinvent the wheel by reimplementing the handshake and overall `WebSocket` protocol, so the only approach I considered was looking for a 3rd party app.
- This approach had 2 drawbacks, however. First, I could not find a well-accepted library which implements the handshake and overhead code (i.e. this was not in the stdlib). Second, I would have had to re-write my game server (which I had already built to rely on TCP) which would have taken considerable effort. 

**3. Using `websockify` as a middleware proxy to connect a traditional TCP server to a `WebSocket`**
- `websockify` "at its most basic level, just translates `WebSocket` traffic to normal socket traffic." 
- This was perfect for my issue, since I could keep my `TCP`-based game server untouched and rely on the proxy server to facilitate the translation between the higher level `WebSocket` protocol and the lower level `TCP`. 
- The link the `node.js` version of the `websockify` server is [here](https://github.com/novnc/websockify-js). (Note: the main version of the project is written in `python` and can be found [here](https://github.com/novnc/websockify). 
- This project was spun out of the `noVPC` project (which is an `HTML5`-based version of a remote-linux desktop) which was solving this exact problem I had (the `VPC` server used `TCP` but the browser uses `WebSockets`). 
- Unfortunately, I had a lot of trouble getting this setup and the =examples found online are relatively few and far between, so I thought it would be helpful to provide the code and how-to of the proofs of concepts I did to get everything working

### Example Using websockify With Instructions

***This repo includes a couple of files, which are described below:***

**1. `echo-server-linux.cpp`**
- A basic `TCP` server running on `Linux` 
- Opens up a Berkeley socket, waits for clients to connect on `port 5000` and send a message and echos back `[client message] + "from the server"`. 
- Complie and run with: `g++ echo-server-linux.cpp -o echo-server-linux` and run with `.\echo-server-linux`.

**2. `echo-server-win.cpp`** 
- A basic `TCP` server running on `Windows`
- Opens up a `WinSock`, waits for clients to connect on `port 5000` and send a message and echos back `[client message]`. 
- Complile and run with: `g++ echo-server-win.cpp -o echo-server-win -lWS2_32` and run with `echo-server-win.exe`. '
- `g++` can be installed for `Windows` by following the instructions [here](https://www3.cs.stonybrook.edu/~alee/g++/g++.html). '
- I made some minor modifications to the code from [here](`https://docs.microsoft.com/en-us/windows/win32/winsock/complete-server-code?redirectedfrom=MSDN`) to build this.

**3. `index.html`** 
- A static `HTML` file with some `JavaScript` embedded. 
- Creates a basic form for the user to type commands, also opens up a `WebSocket` which points to `localhost:27016`. Whatever the user types is sent to the `WebSocket` and logged in `red`, whatever is recieved from the `WebSocket` is logged in `blue`. 
- The `index.html` file is mostly taken from [Miguel Grinberg's example](https://github.com/miguelgrinberg/flask-sock/tree/main/example/templates), which he built to demonstrate how to use `flask`'s `WebSocket` functionality (`flask-sock`).

***Then, we need to do the following steps to setup the proof of concept:***

**1. Run the `TCP` echo server and launch a front-end server to serve the static web app (`index.html`).** 
- Open up a terminal and run one of the echo servers (either the Linux or the Windows one - compilation and running instructions defined above). 
- Open up another terminal and launch a front-end server to serve the `index.html` file. You can do this with any basic web server (I have run this proof of concept with the off the shelf `react-create-app` code / dev-server for instance). `websockify` also includes functionality to serve static files (in addition to being a proxy), so I will use that functionality below rather than spinning up a separate server to provide the static content.

**2. Download and configure `websockify`.**
- We are going to use the `node.js` version of `websockify`. I have only tested this on `Windows`, though I would expect it should work anywhere `node.js` runs. [This blog from 2014](https://barkingbogart.wordpress.com/2014/10/08/use-websockify-in-windows/) was very helpful in building this example.
- Download the `zip` file by clicking [this link](https://github.com/novnc/websockify-js/archive/refs/heads/master.zip) and extract the files. 
- Move the `websockify-js-master` directory to be at the same level as this `README.md`. 
- Navigate to `\websockify-js-master\websockify`. 
- Run `npm install` to install the dependencies. 
 
**3. Run `websockify`** 
- Run the server by calling: `node websockify.js --web [DIR] SOURCE_ADDR:PORT TARGET_ADDR:PORT` (in our case `node websockify.js --web ../../ localhost:27016 localhost:5000` in the `\websockify-js-master\websockify` folder. 
- The `--web [DIR]` command runs a mini web server on the same port where the proxy is running (`../../` is where `index.html` is located). 
- The `localhost:27016 localhost:5000` command instructs `websockify` to accept `WebSocket` connections on `27016` and forward them to traditional `TCP` sockets on `5000`.

**4. Check the proof of concept** 
- Open a browser and type `http://localhost:27016/index.html`.
- A basic website show appear with a `form` to type. Type any text and click enter. 
- The text should show up in `red` as `>>> [text]` and then again in `blue` as `<<< [text]`. The `red` message is what was sent to the `WebSocket`. The `blue` message is what was recieved from the `WebSocket`. You should also see the `TCP` servers are logging some items in the terminals which demonstrate that they are being pinged.
