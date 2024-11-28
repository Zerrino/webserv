<?php
$path = $_SERVER["DOCUMENT_ROOT"]; // Chemin du dossier à lister, ici dossier racine (data)
$files = scandir($path);

$result = [];
foreach ($files as $file) {
	if ($file !== '.' && $file !== '..') {
		$fullPath = rtrim($path, '/') . '/' . $file;
		$result[] = [
			'name' => $file,
			'type' => is_dir($fullPath) ? 'directory' : 'file'
		];
	}
}

header('Content-Type: application/json');
echo json_encode($result);
