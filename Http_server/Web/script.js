// alert("Bui Thi Ly");
const ws = new WebSocket("http://127.0.0.1:8080");
ws.onmessage = () => location.reload();  // Reload khi nhận tín hiệu từ server
