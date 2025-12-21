#include <node.h>

using namespace v8;

// Your C function
void c_hello() {
    printf("hello world\n");
}

// Wrapper so JavaScript can call it
void CHello(const FunctionCallbackInfo<Value>& args) {
    c_hello();
}

// This gets called when JavaScript does require('./hello.node')
void Initialize(Local<Object> exports) {
    NODE_SET_METHOD(exports, "c_hello", CHello);
}

// Register the addon
NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize)

/*
 * Compile the addon:
 * g++ -shared -fPIC -std=c++17 \
 *     $(pkg-config --cflags --libs nodejs-22) \
 *     addon.cpp -o hello.node
 * 
 * Create test.js:
 * const hello = require('./hello.node');
 * console.log('Calling C function from JavaScript...');
 * hello.c_hello();
 * console.log('Done!');
 * 
 * Run with regular node:
 * node test.js
 * 
 * Or run with your embedded runtime (the simple 5-line version):
 * ./simple test.js
 */
