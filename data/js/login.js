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
        localStorage.setItem("auth", 1);
        this.form.submit();
        console.log("clicked");
      }
    });
  }
  validateFields(field) {
    if (field.type == "password") {
		if (field.value.trim() === "") {
			this.setStatus(
				field,
				`${field.name} should not be blank`,
				"error"
			  );
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

const form = document.querySelector(".loginForm");
if (form) {
  const fields = ["email", "password"];
  const validator = new Login(form, fields);
}
