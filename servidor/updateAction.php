<?php     
  require 'database.php';
  
  if (!empty($_POST)) {
    $Direccion = $_POST['direction'];
      
    // insert data
    $pdo = Database::connect();
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    $sql = "UPDATE direccion SET Direccion = ? WHERE ID = 0";
    $q = $pdo->prepare($sql);
    $q->execute(array($Direccion));
    Database::disconnect();
    header("Location: index.php");
  }
?>