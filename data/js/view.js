/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   view.js                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdelvign <gdelvign@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 21:25:45 by gdelvign          #+#    #+#             */
/*   Updated: 2024/11/20 21:33:37 by gdelvign         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

const viewModal = document.querySelector("#viewFiles-modal");

const toggleViewFilesModal = () => {
  viewModal.classList.remove("pointer-events-none");
  if (viewModal.classList.contains("hidden"))
    viewModal.classList.remove("hidden");
  else {
    viewModal.classList.add("hidden");
  }
};
