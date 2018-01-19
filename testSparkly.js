const canvas = document.getElementById('canvas');
const context = canvas.getContext('2d');

// Canvas resizing from http://stackoverflow.com/a/43364730/2142626
const width = canvas.clientWidth;
const height = canvas.clientHeight;
if (canvas.width !== width || canvas.height !== height) {
    canvas.width = width;
    canvas.height = height;
}

let mousePos;

canvas.addEventListener('mousemove', (e) => {
    mousePos = { x: e.clientX, y: e.clientY };
});

Module.addOnPostRun(() => {
    const sparkly = new Module.Sparkly(width, height);

    function drawFrame() {
        if (mousePos) {
            const frame = sparkly.drawFrame(mousePos.x, mousePos.y);
            if (frame) {
                const imageArray = new Uint8ClampedArray(frame.buffer, frame.byteOffset, frame.byteLength);
                const imageData = new ImageData(imageArray, width, height);
                context.putImageData(imageData, 0, 0);
            }
        }
        window.requestAnimationFrame(drawFrame);
    }
    
    window.requestAnimationFrame(drawFrame);

});
