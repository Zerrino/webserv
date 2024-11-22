/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   login.js                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 21:26:14 by gdelvign          #+#    #+#             */
/*   Updated: 2024/11/22 09:45:51 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

class Login {
  constructor(form, fields) {
    this.form = form;
    this.fields = fields;
    this.validateonSubmit();
  }

  validateonSubmit() {
    this.form.addEventListener("submit", (e) => {
      e.preventDefault();
      var error = 0;
      this.fields.forEach((field) => {
        const input = document.querySelector(`#${field}`);
        if (this.validateFields(input) == false) {
          error++;
        }
      });
      if (error == 0) {
        const formData = new FormData(this.form);
        const params = Object.fromEntries(formData.entries());
        params.action = "cookieUpdate";
        createRequest(params);
      }
    });
  }

  validateFields(field) {
    if (field.type == "password") {
      if (field.value.trim() === "") {
        this.setStatus(field, `${field.name} should not be blank`, "error");
        return false;
      }
      if (field.value.length < 8) {
        this.setStatus(
          field,
          `${field.name} must be at least 8 characters`,
          "error"
        );
        return false;
      } else {
        this.setStatus(field, null, "success");
        return true;
      }
    }
  }

  setStatus(field, message, status) {
    const errorMessage = field.parentElement.lastElementChild;

    if (status == "success") {
      if (errorMessage) {
        errorMessage.innerText = "";
      }
      field.classList.remove("input-error");
    }
    if (status == "error") {
      errorMessage.innerText = message;
      field.classList.add("input-error");
    }
  }
}

const showModal = (status) => {
  let modal;
  if (status === "success") {
    modal = document.getElementById("success-modal");
  } else {
    modal = document.getElementById("failure-modal");
  }
  const btn = modal.getElementsByTagName("button")[0];
  btn.addEventListener("click", (e) => {
    e.preventDefault();
    window.location.replace("/");
  });
  modal.classList.remove("hidden");
};

async function createRequest(data) {
  try {
    const request = await fetch("/ressources/database/profiles.txt", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify(data),
    });
    if (request.ok && request.status === 204) {
      localStorage.setItem("auth", 1);
      localStorage.setItem("email", data.email);
      localStorage.setItem("username", data.email.split("@")[0]);
      localStorage.setItem("initial", data.email[0].toUpperCase());
      const response = await fetch("/src/dashboard.html");
      if (!response.ok)
        console.log("error");
      else if (response.ok)
        window.location.href = "/src/dashboard.html";
    } else if (request.status === 404) {
      showModal("error");
    } else {
      throw new Error(`Server error: ${request.status}`);
    }
  } catch (error) {
    console.error("An error occurred:", error.message);
  }
}

const form = document.querySelector(".loginForm");
if (form) {
  const fields = ["email", "password"];
  const validator = new Login(form, fields);
}

document.getElementById("eye-icon").addEventListener("click", (e) => {
  const input = document.getElementById("password");
  if (input.type === "text") {
    input.type = "password";
  } else input.type = "text";
});
