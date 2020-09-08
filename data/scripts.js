const orderList = document.querySelector('.ordersList');
const addBtn = document.querySelector('.addButton button');
const clearBtn = document.querySelector('.clearButton button');
const submitBtn = document.querySelector('.submitButton button');

orderList.innerHTML = ' ';

function createOne() {
    const divc = document.createElement("div");
    divc.setAttribute("id", "MoveNum" + position);
    divc.classList.add("moveCard");

    const numberIndex = document.createElement("h2");
    numberIndex.innerHTML = position + ".";
    numberIndex.classList.add("moveCardItem");


    const distanceSel = document.createElement("INPUT");
    distanceSel.setAttribute("type", "number");
    distanceSel.setAttribute("name", "distance");
    distanceSel.setAttribute("placeholder", "Distance");
    distanceSel.classList.add("moveCardItem");
    distanceSel.required = true;
    //distanceSel.defaultValue = 0;


    const speedSel = document.createElement("INPUT");
    speedSel.setAttribute("type", "number");
    speedSel.setAttribute("name", "speed");
    speedSel.setAttribute("min", "0");
    speedSel.setAttribute("placeholder", "Speed");
    speedSel.classList.add("moveCardItem");
    speedSel.required = true;
    //speedSel.defaultValue = 0;

    const acceleratedLabel = document.createElement("label");
    acceleratedLabel.setAttribute("for", "accelerated?");
    acceleratedLabel.innerHTML = "Accelerated: ";
    acceleratedLabel.classList.add("moveCardItem");

    const acceleratedSel = document.createElement("INPUT");
    acceleratedSel.setAttribute("type", "checkbox");
    acceleratedSel.setAttribute("name", "mType");
    acceleratedSel.classList.add("moveCardItem");
    acceleratedSel.classList.add("accelMove");
    acceleratedSel.setAttribute("value", position);
    acceleratedSel.onclick = (function() {
        document.getElementById("debug").innerHTML = "<p>" + acceleratedSel.value + "</p>";
        togglebox1 = document.getElementById("my-boxA" + acceleratedSel.value);
        togglebox2 = document.getElementById("my-boxB" + acceleratedSel.value);
        togglebox1.style.display = (togglebox1.style.display != 'inline') ? 'inline' : 'none';
        togglebox2.style.display = (togglebox2.style.display != 'inline') ? 'inline' : 'none';
    });


    const accelerationSel = document.createElement("INPUT");
    accelerationSel.setAttribute("type", "number");
    accelerationSel.setAttribute("name", "acceleration");
    accelerationSel.setAttribute("min", "0");
    accelerationSel.setAttribute("placeholder", "Acceleration");
    accelerationSel.setAttribute("id", "my-boxB" + position);
    accelerationSel.classList.add("my-box");
    accelerationSel.classList.add("moveCardItem");
    accelerationSel.required = true;
    //accelerationSel.defaultValue = 0;

    const eraseButton = document.createElement("button");
    eraseButton.innerHTML = "✖";
    eraseButton.classList.add("eraseClass");
    eraseButton.onclick = function() { divc.parentNode.removeChild(divc); };


    divc.appendChild(numberIndex);
    divc.appendChild(distanceSel);
    divc.appendChild(speedSel);
    //divc.appendChild(acceleratedLabel);
    //divc.appendChild(acceleratedSel);
    divc.appendChild(accelerationSel);
    divc.appendChild(eraseButton);

    orderList.appendChild(divc);
}


let position = 1;
addBtn.onclick = function() { createOne(position);
    position++; };
clearBtn.onclick = function() { orderList.innerHTML = ' ';
    position = 1; };
//submitBtn.onlick = function() {orderList.innerHTML= ' '; position=1;};
document.getElementById("baseform").onsubmit = function(event) {
    //var items = {};
    event.preventDefault()
    document.getElementById("debug").innerHTML = " ";
    var form = document.getElementsByClassName("moveCard");
    var i = 0;
    let Url = "http://192.168.4.1/multi?";
    for (i = 0; i < form.length; i++) {
        var inputs = form[i].childNodes;
        //document.getElementById("debug").innerHTML = document.getElementById("debug").innerHTML + form[i];
        //form[i].style.backgroundColor = "red";
        var j;
        var distanceInput = inputs[1].value;
        var speedInput = inputs[2].value;
        var accelerationInput = inputs[3].value;
        var mtypeInput = false;
        if (accelerationInput != 0) { mtypeInput = true };


        document.getElementById("debug").innerHTML = document.getElementById("debug").innerHTML + "<br>" + distanceInput + "," + speedInput + "," + mtypeInput + "," + accelerationInput;
        if (mtypeInput == true) {
            Url = Url + "accel=" + distanceInput + ";" + speedInput + ";" + accelerationInput + "&";
        } else {
            Url = Url + "cons=" + distanceInput + ";" + speedInput + "&";
        }
    };
    Url = Url.substring(0, Url.length - 1);
    /**document.getElementById("debug").innerHTML = Url;**/
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", Url, true);
    xhttp.send(null);
}