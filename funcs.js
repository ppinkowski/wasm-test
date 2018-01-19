mergeInto(LibraryManager.library, {
    consoleLog: function(messagePtr) {
        const message = Pointer_stringify(messagePtr);
        console.log(message);
    },
    addEventLstnr: function(event, callback) {
        console.log(Pointer_stringify(event));
        document.addEventListener(Pointer_stringify(event), (e) => {
            Runtime.dynCall('vii', callback, [e.clientX, e.clientY]);
        });
    }
});