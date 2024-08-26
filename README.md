To run: 
Compile with 'make' in directory terminal
Run PrimaryLikesServer first with './PrimaryLikesServer'
Run ParentProcess with './ParentProcess'

'make clean' to clean executables.

This project is about Eventual Consistency of data, and we were tasked with modeling a simple YouTube likes system where multiple processes can send data to a primary server to update like counts for a video.

My thought process to complete this project was to first get a firm understanding of using sockets for IPC. I chose to use a UDP socket because the nature of the project basically only requires communication from the client to server.
Resources I used to learn about sockets and the implementation are the following:
https://youtu.be/lUyaV4haBUE?si=wQ3W_KNRSbytSQJj
https://users.cs.jmu.edu/bernstdh/web/common/lectures/summary_unix_udp.php
