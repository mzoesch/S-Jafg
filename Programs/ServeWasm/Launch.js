// Copyright mzoesch. All rights reserved.

const http = require('http-server');

const options = {
    root: '../../bin/wasm-x86/cl-dbg/Runtime',
    headers: {
        'Cross-Origin-Embedder-Policy': 'require-corp',
        'Cross-Origin-Opener-Policy': 'same-origin',
    },
}

const server = http.createServer(options);
server.listen(8080, () => {
    console.log('Server running at http://localhost:8080/');
});
