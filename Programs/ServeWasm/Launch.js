const http = require('http-server');

const options = {
    root: '../../Binaries/Wasm-wasm32/Debug-Client/Runtime',
    headers: {
        'Cross-Origin-Embedder-Policy': 'require-corp',
        'Cross-Origin-Opener-Policy': 'same-origin',
    },
}

const server = http.createServer(options);
server.listen(8080, () => {
    console.log('Server running at http://localhost:8080/');
});
