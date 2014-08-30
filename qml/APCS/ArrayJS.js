var myArray = new Array()

function addItem(item) {
    myArray.push(item)
}

function clear() {
    for (var i = 0; i < myArray.length; i++) {
        myArray[i].visible = false;
        myArray[i].destroy();
    }

    myArray = [];
}
