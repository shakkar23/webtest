// Now you can call your function


var result = Module.onRuntimeInitialized = () => {
    const result = Module._add(1, 1);
    console.log(result);
};