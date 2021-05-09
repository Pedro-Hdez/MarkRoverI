<!DOCTYPE html>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta charset="utf-8">
    <link rel="stylesheet" href="style.css">
    <script src="jquery.min.js"></script>
  </head>
  <body>
    <h1>CONTROLES MARK ROVER l</h1>
    <div class="container">
        <form action="updateAction.php" method="post" id="FORWARD">
        <input type="hidden" name="direction" value="forward"/>    
        </form>

        <form action="updateAction.php" method="post" id="LEFT">
        <input type="hidden" name="direction" value="left"/>    
        </form>

        <form action="updateAction.php" method="post" id="STOP">
        <input type="hidden" name="direction" value="stop"/>    
        </form>
        
        <form action="updateAction.php" method="post" id="RIGHT">
        <input type="hidden" name="direction" value="right"/>
        </form>

        <form action="updateAction.php" method="post" id="REVERSE">
        <input type="hidden" name="direction" value="reverse"/>
        </form>

        <form action="updateAction.php" method="post" id="AUTOMATIC">
        <input type="hidden" name="direction" value="atm"/>
        </form>

        <input type="image" class="directionButton" src="assets/up-arrow.png" alt="submit" form="FORWARD"/><br><br>
        <input type="image" class="directionButton" src="assets/left-arrow.png" alt="submit" form="LEFT"/>
        <input type="image" class="directionButton" src="assets/stp-pressed.png" alt="submit" form="STOP"/>
        <input type="image" class="directionButton" src="assets/right-arrow.png" alt="submit" form="RIGHT"/><br><br>
        <input type="image" class="directionButton" src="assets/down-arrow.png" alt="submit" form="REVERSE"/>
        <br><br><br>
        <input type="image" class="atmPilotButton" src="assets/atm.png" alt="submit" form="AUTOMATIC"/>
    </div>
    <script src="script.js"></script>
  </body>
</html>