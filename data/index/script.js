
// Sélectionner le bouton et le paragraphe de message
const bouton = document.getElementById('bouton');
const message = document.getElementById('message');

// Ajouter un écouteur d'événement au bouton
bouton.addEventListener('click', () => {
  message.textContent = "Vous avez cliqué sur le bouton !";
});

