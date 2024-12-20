/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   view.js                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdelvign <gdelvign@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 21:25:45 by gdelvign          #+#    #+#             */
/*   Updated: 2024/12/20 22:58:28 by gdelvign         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

const viewModal = document.querySelector("#viewFiles-modal");

async function getAllUploadedFiles() {
  try {
    const response = await fetch("/ressources/uploads");
	console.log(response);
    const files = await response.text();
    console.log(files);
  } catch (err) {
    console.error(err);
  }
}

const toggleViewFilesModal = () => {
  viewModal.classList.remove("pointer-events-none");
  if (viewModal.classList.contains("hidden")) {
    viewModal.classList.remove("hidden");
    getAllUploadedFiles();
  } else {
    viewModal.classList.add("hidden");
  }
};
