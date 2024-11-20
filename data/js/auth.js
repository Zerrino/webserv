/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   auth.js                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdelvign <gdelvign@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 21:27:00 by gdelvign          #+#    #+#             */
/*   Updated: 2024/11/20 21:27:24 by gdelvign         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

class Auth {
  constructor() {
    document.querySelector("body").classList.add = "hidden";
    const auth = localStorage.getItem("auth");
    this.validateAuth(auth);
  }
  validateAuth(auth) {
    if (auth != 1) {
      window.location.replace("/");
    } else {
      document.querySelector("body").classList.remove = "hidden";
    }
  }
  logOut() {
    localStorage.removeItem("auth");
    window.location.replace("/");
  }
}

const auth = new Auth();

document.querySelector(".logout").addEventListener("click", (e) => {
    auth.logOut();
});