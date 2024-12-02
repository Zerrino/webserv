<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Formulaire PHP</title>
</head>
<body>
    <h1>Entrez votre nom</h1>
    <form method="POST" action="">
        <label for="name">Nom :</label>
        <input type="text" id="name" name="name" required>
        <button type="submit">Envoyer</button>
    </form>

    <?php
    // Vérifiez si le formulaire a été soumis
    if ($_SERVER["REQUEST_METHOD"] === "POST") {
        // Récupérez la valeur du champ "name"
        $name = htmlspecialchars($_POST["name"]);
        echo "<p>Bonjour, $name !</p>";
    }
    ?>
</body>
</html>
