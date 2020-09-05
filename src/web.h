const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>

<html>

<head>
    <meta name='viewport' content='width=device-width,         initial-scale=1.0,         user-scalable=no' />
    <meta charset="utf-8" />


    <title>ESP8266 turntable Test</title>
</head>

<body>

    <form id="baseform">
        <div class="ordersList">

        </div>
        <div class="submitButton">
            <button type="submit">Submit</button>
        </div>
    </form>


    <div class="addButton">
        <button>Add movement</button>
    </div>
    <div class="clearButton">
        <button>Clear</button>
    </div>


    <div id="debug">
    </div>

</body>


<script type="text/javascript">

    const orderList = document.querySelector('.ordersList');
    const addBtn = document.querySelector('.addButton button');
    const clearBtn = document.querySelector('.clearButton button');
    const submitBtn = document.querySelector('.submitButton button');

    orderList.innerHTML = ' ';

    function createOne() {
        const divc = document.createElement("div");
        divc.setAttribute("id", "MoveNum" + position);
        divc.classList.add("moveCard");

        const distanceLabel = document.createElement("label");
        distanceLabel.setAttribute("for", "distance");
        distanceLabel.innerHTML = "Distance: ";

        const distanceSel = document.createElement("INPUT");
        distanceSel.setAttribute("type", "number");
        distanceSel.setAttribute("name", "distance");
        distanceSel.required = true;
        distanceSel.defaultValue = 0;

        const speedLabel = document.createElement("label");
        speedLabel.setAttribute("for", "speed");
        speedLabel.innerHTML = "Speed: ";

        const speedSel = document.createElement("INPUT");
        speedSel.setAttribute("type", "number");
        speedSel.setAttribute("name", "speed");
        speedSel.setAttribute("min", "0");
        speedSel.required = true;
        speedSel.defaultValue = 0;

        const acceleratedLabel = document.createElement("label");
        acceleratedLabel.setAttribute("for", "accelerated");
        acceleratedLabel.innerHTML = "Accelerated: ";

        const acceleratedSel = document.createElement("INPUT");
        acceleratedSel.setAttribute("type", "checkbox");
        acceleratedSel.setAttribute("name", "mType");
        acceleratedSel.classList.add("accelMove");
        acceleratedSel.setAttribute("value", position);
        acceleratedSel.onclick = (function () {
            document.getElementById("debug").innerHTML = "<p>" + acceleratedSel.value + "</p>";
            togglebox1 = document.getElementById("my-boxA" + acceleratedSel.value);
            togglebox2 = document.getElementById("my-boxB" + acceleratedSel.value);
            togglebox1.style.display = (togglebox1.style.display != 'inline') ? 'inline' : 'none';
            togglebox2.style.display = (togglebox2.style.display != 'inline') ? 'inline' : 'none';
        });

        const accelerationLabel = document.createElement("label");
        accelerationLabel.setAttribute("for", "acceleration");
        accelerationLabel.innerHTML = "Acceleration: ";
        accelerationLabel.setAttribute("id", "my-boxA" + position);
        accelerationLabel.classList.add("my-box");

        const accelerationSel = document.createElement("INPUT");
        accelerationSel.setAttribute("type", "number");
        accelerationSel.setAttribute("name", "acceleration");
        accelerationSel.setAttribute("min", "0");
        accelerationSel.setAttribute("id", "my-boxB" + position);
        accelerationSel.classList.add("my-box");
        accelerationSel.required = true;
        accelerationSel.defaultValue = 0;

        const eraseButton = document.createElement("button");
        eraseButton.innerHTML = "X";
        eraseButton.onclick = function () { divc.parentNode.removeChild(divc); };



        divc.appendChild(distanceLabel);
        divc.appendChild(distanceSel);
        divc.appendChild(speedLabel);
        divc.appendChild(speedSel);
        divc.appendChild(acceleratedLabel);
        divc.appendChild(acceleratedSel);
        divc.appendChild(accelerationLabel);
        divc.appendChild(accelerationSel);
        divc.appendChild(eraseButton);

        orderList.appendChild(divc);
    }


    let position = 1;
    addBtn.onclick = function () { createOne(position); position++; };
    clearBtn.onclick = function () { orderList.innerHTML = ' '; position = 1; };
    //submitBtn.onlick = function() {orderList.innerHTML= ' '; position=1;};
    document.getElementById("baseform").onsubmit = function (event) {
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
            var speedInput = inputs[3].value;
            var mtypeInput = inputs[5].checked;
            var accelerationInput = inputs[7].value;


            document.getElementById("debug").innerHTML = document.getElementById("debug").innerHTML + "<br>" + distanceInput + "," + speedInput + "," + mtypeInput + "," + accelerationInput;
            if (mtypeInput == true) {
                Url = Url + "accel=" + distanceInput + "-" + speedInput + "-" + accelerationInput + "&";
            } else {
                Url = Url + "cons=" + distanceInput + "-" + speedInput + "&";
            }
        };
        Url = Url.substring(0, Url.length - 1);
        document.getElementById("debug").innerHTML = Url;
        var xhttp = new XMLHttpRequest();
        xhttp.open("GET", Url)
    }
</script>

</html>
)=====";