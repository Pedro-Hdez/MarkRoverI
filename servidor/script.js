'use strict';

$(function(){
    $("input").click(function(e){
        e.preventDefault();
        let value; 
        if (this.src.includes("atm.png")){
            value = "stop"
        }
        else{
            value = $(this)[0].form[0].value;
        }
        console.log(value)
        $.ajax
        ({
            type: "POST",
            url:"updateAction.php",
            data:{"direction":value}
        })
    })
})

const directionButtons = document.querySelectorAll('.directionButton')
let pressed = directionButtons[2]

const directionButtonPressed = function(){
    const image = this.src.split('/')[5].split('.')[0];
    let newImage = ""
    if (image.includes("-pressed")){
        return;
    }
    else{
        newImage = "assets/" + image + "-pressed.png";
        pressed.src = pressed.src.replace('-pressed', "");
        pressed = this;
    }
    this.src = newImage;
}

const atmPilotButtonPressed = function(){
    const image = this.src.split('/')[5].split('.')[0];
    let newImage = ""
    if (!image.includes("-pressed")){
        for(let i = 0 ; i < directionButtons.length ; i++){
            directionButtons[i].disabled = true;
        }    
        pressed.src = pressed.src.replace('-pressed', "");
        this.src = "assets/" + image + "-pressed.png"    
        pressed = this
    }
    else{
        for(let i = 0 ; i < directionButtons.length ; i++){
            directionButtons[i].disabled = false;
        }    
        this.src = this.src.replace("-pressed", "")
        pressed = directionButtons[2]
        pressed.src = "assets/" + pressed.src.split('/')[5].split('.')[0] + "-pressed.png"

        return;
    }
}

for(let i = 0 ; i < directionButtons.length ; i++){
    directionButtons[i].addEventListener('click', directionButtonPressed);
}

document.querySelector(".atmPilotButton").addEventListener("click", atmPilotButtonPressed)