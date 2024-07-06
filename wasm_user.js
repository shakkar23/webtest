

let app = document.getElementById("game");
console.log(app);

app.style.position = "absolute";
app.style.left = 0 + 'px';
app.style.top = 0 + 'px';

app.style.width = (window.innerWidth) + 'px';
app.style.height = (window.innerHeight) + 'px';

function resize_canvas() {
    app.width = window.innerWidth / 3;
    app.height = window.innerHeight / 3;
    app.style.width = (window.innerWidth + 1) + 'px';
    app.style.height = (window.innerHeight + 1) + 'px';
} resize_canvas();

console.log(app);


let ctx = app.getContext("2d");
let prev_time = 0.0;

// wait until the wasm thing loaded before we start updating the screen
let result = Module.onRuntimeInitialized = () => {

    requestAnimationFrame((timestamp) => {
        prev_time = timestamp;
        window.requestAnimationFrame(update);
    });

    // upper case versions of WASD are allocated for the arrow keys, everything else should be the same though
    // just dont hold the shift key while playing tetris, and use my controls i guess
    addEventListener('keydown', (e) => {
        if (!e.repeat) {
            if (e.key == "ArrowUp") {
                Module._keydown('\x00'.charCodeAt(0));

            } else if (e.key == "ArrowLeft") {
                Module._keydown('\x01'.charCodeAt(0));

            } else if (e.key == "ArrowDown") {
                Module._keydown('\x02'.charCodeAt(0));

            } else if (e.key == "ArrowRight") {
                Module._keydown('\x03'.charCodeAt(0));

            } else
                Module._keydown(e.key.toLowerCase().charCodeAt(0));
            console.log(e.key);
        }
    });

    addEventListener('keyup', (e) => {
        if (!e.repeat) {
            if (e.key == "ArrowUp") {
                Module._keyup('\x00'.charCodeAt(0));
            } else if (e.key == "ArrowLeft") {
                Module._keyup('\x01'.charCodeAt(0));
            } else if (e.key == "ArrowDown") {
                Module._keyup('\x02'.charCodeAt(0));
            } else if (e.key == "ArrowRight") {
                Module._keyup('\x03'.charCodeAt(0));
            } else
                Module._keyup(e.key.toLowerCase().charCodeAt(0));
            console.log(e.key);
        }
    });

};

// this is also the rendering code
function update(time) {
    let dt = time - prev_time;

    // the update function does both rendering as well as logic
    let screen_ptr = Module._update(dt / 1000, app.width, app.height);
    let arr = new Uint8ClampedArray(Module.HEAPU8.buffer, screen_ptr, app.width * app.height * 4)
    let wasm_screen = new ImageData(arr, app.width, app.height);

    ctx.putImageData(wasm_screen, 0, 0);
    // request to keep updating
    requestAnimationFrame(update);
    prev_time = time;
}