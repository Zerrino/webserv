<?php
session_start();

// Chemin du fichier CSV
$csvFile = 'users.csv';

// Fonction pour lire les utilisateurs depuis le fichier CSV
function readUsersFromCSV($csvFile) {
    $users = [];
    if (($handle = fopen($csvFile, 'r')) !== false) {
        $header = fgetcsv($handle); // Lire l'en-tête
        while (($data = fgetcsv($handle)) !== false) {
            $users[$data[0]] = $data[1]; // Nom d'utilisateur => Mot de passe haché
        }
        fclose($handle);
    }
    return $users;
}

// Lire les utilisateurs depuis le fichier CSV
$users = readUsersFromCSV($csvFile);

// Gestion du formulaire
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $username = trim($_POST['username']);
    $password = $_POST['password'];

    if (isset($users[$username]) && password_verify($password, $users[$username])) {
        // Stocker l'utilisateur dans la session
        $_SESSION['username'] = $username;
        header("Location: welcome.php");
        exit();
    } else {
        $error = "Nom d'utilisateur ou mot de passe incorrect.";
    }
}
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Login</title>
</head>
<body>
    <h1>Connexion</h1>
    <?php if (!empty($error)): ?>
        <p style="color: red;"><?= htmlspecialchars($error) ?></p>
    <?php endif; ?>
    <form method="POST" action="">
        <label for="username">Nom d'utilisateur :</label>
        <input type="text" id="username" name="username" required>
        <br>
        <label for="password">Mot de passe :</label>
        <input type="password" id="password" name="password" required>
        <br>
        <button type="submit">Se connecter</button>
    </form>
</body>
</html>
