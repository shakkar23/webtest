// Now you can call your function


var result = Module.onRuntimeInitialized = () => {
    let buf = new Int32Array(1);
    const result = Module._add(buf.data);
    console.log(buf[0]);
};

