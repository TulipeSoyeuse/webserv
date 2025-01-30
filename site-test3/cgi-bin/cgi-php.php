<?php
session_start();

if (!isset($_SESSION['bg_color'])) {
    $_SESSION['bg_color'] = sprintf("#%06X", mt_rand(0, 0xFFFFFF));
}

$color = $_SESSION['bg_color'];
?>
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Fond Persistant</title>
    <style>
        body {
            background-color: <?php echo $color; ?>;
            color: white;
            font-family: Arial, sans-serif;
            text-align: center;
            margin: 0;
            padding: 50px;
            transition: background 0.5s ease;
        }
        button {
            padding: 10px 20px;
            font-size: 18px;
            cursor: pointer;
            border: none;
            background: white;
            color: black;
            border-radius: 5px;
            transition: 0.3s;
        }
        button:hover {
            background: #ddd;
        }
    </style>
</head>
<body>
    <h1>Couleur du fond : <?php echo $color; ?></h1>
    <p>La couleur restera la même jusqu'à ce que vous la changiez.</p>
    <form method="post">
        <button type="submit" name="reset">Changer la couleur</button>
    </form>
    <?php
    if (isset($_POST['reset'])) {
        $_SESSION['bg_color'] = sprintf("#%06X", mt_rand(0, 0xFFFFFF));
        header("Location: ".$_SERVER['PHP_SELF']);
        exit;
    }
    ?>

    <br>
    <a href="/">Retour à l'accueil</a>
</body>
</html>

