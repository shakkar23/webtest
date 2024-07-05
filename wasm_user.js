

let app;
let ctx;
let prev_time = 0.0;

// wait until the wasm thing loaded before we start updating the screen
let result = Module.onRuntimeInitialized = () => {

    app = document.getElementById("game");
    ctx = app.getContext("2d");

    requestAnimationFrame((timestamp) => {
        prev_time = timestamp;
        window.requestAnimationFrame(update);
    });
};


// this is also the rendering code
function update(time) {
    let dt = time - prev_time;


    let screen_ptr = Module._render(app.width, app.height);
    let arr = new Uint8ClampedArray(Module.HEAPU8.buffer, screen_ptr, app.width * app.height * 4)
    let wasm_screen = new ImageData(arr, app.width, app.height);

    ctx.putImageData(wasm_screen, 0, 0);
    // request to keep updating
    requestAnimationFrame(update);
}