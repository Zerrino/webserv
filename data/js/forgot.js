class ChangePassword {
  constructor(form, fields) {
    this.form = form;
    this.fields = fields;
    this.validateonSubmit();
  }

  validateonSubmit() {
    this.form.addEventListener("submit", (e) => {
      e.preventDefault();
      if (this.validateFields() == false) {
        return;
      } else {
        const formData = new FormData(this.form);
        createRequest(formData);
      }
    });
  }

  validateFields() {
    const password = document.getElementById("password");
    const repeated = document.getElementById("cpassword");

    if (password.value.trim() === "") {
      this.setStatus(password, "Password field should not be blank", "error");
      return false;
    } else if (password.value.length < 8) {
      this.setStatus(
        password,
        "Password must be at least 8 characters",
        "error"
      );
      return false;
    } else this.setStatus(password, "", "success");
    return this.checkPasswordEquality(password, repeated);
  }

  checkPasswordEquality(password, repeated) {
    if (password.value != repeated.value) {
      this.setStatus(repeated, "Passwords are not corresponding", "error");
      return false;
    } else {
      this.setStatus(password, "", "success");
      return true;
    }
  }

  setStatus(field, message, status) {
    const errorMessageDiv = field.parentElement.lastElementChild;

    if (status == "success") {
      if (errorMessageDiv) {
        errorMessageDiv.innerText = "";
      }
      field.classList.remove("input-error");
    }
    if (status == "error") {
      errorMessageDiv.innerText = message;
      field.classList.add("input-error");
    }
  }
}

const showModal = (status) => {
  let modal;
  if (status === "success") {
    modal = document.getElementById("success-modal");
  } else modal = document.getElementById("failure-modal");
  const btn = modal.getElementsByTagName("button")[0];
  btn.addEventListener("click", (e) => {
    e.preventDefault();
    window.location.replace("/");
  });
  modal.classList.remove("hidden");
};

async function createRequest(data) {
  try {
    const request = await fetch(
      "/data/ressources/database/",
      {
        method: "POST",
        headers: {
          "Content-Type": "application/x-www-form-urlencoded",
        },
        body: JSON.stringify(data),
      }
    );
    if (request.ok) {
      showModal("success");
    } else if (request.status === 404) showModal("error");
    else throw new Error(`Server error: ${request.status}`);
  } catch (error) {
    console.error("An error occurred:", error.message);
  }
}

const form = document.getElementById("recovery-form");
if (form) {
  const fields = ["email", "password", "cpassword"];
  const validator = new ChangePassword(form, fields);
}
