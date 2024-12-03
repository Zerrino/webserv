/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   navBar.js                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdelvign <gdelvign@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 20:27:04 by gdelvign          #+#    #+#             */
/*   Updated: 2024/11/21 11:59:22 by gdelvign         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

var toggleOpen = document.getElementById("toggleOpen");
var toggleClose = document.getElementById("toggleClose");
var collapseMenu = document.getElementById("collapseMenu");

function handleClick() {
  if (collapseMenu.style.display === "block") {
    collapseMenu.style.display = "none";
  } else {
    collapseMenu.style.display = "block";
  }
}

toggleOpen.addEventListener("click", handleClick);
toggleClose.addEventListener("click", handleClick);

document.addEventListener("DOMContentLoaded", function () {
  const currentPath = window.location.pathname;
  const links = document.querySelectorAll("#collapseMenu a");

  links.forEach((link) => {
    if (link.pathname === currentPath) {
      link.classList.remove("text-gray-500");
      link.classList.add("text-teal-600");
    } else {
      link.classList.remove("text-teal-600");
      link.classList.add("text-gray-500");
    }
  });
});

const username = localStorage.getItem("username");
const email = localStorage.getItem("email");
const initial = localStorage.getItem("initial");
document.getElementById("session-avatar").innerHTML = initial;
document.getElementById("session-username").innerHTML = username;
document.getElementById("session-email").innerHTML = email;