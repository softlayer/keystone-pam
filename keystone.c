#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>

#define TEST_USER "tester"

PAM_EXTERN int pam_sm_setcred(pam_handle_t *pamh, int flags, 
		int argc, const char **argv) {
	printf("** pam_pm_setcred\n");
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, 
		int argc, const char **argv) {
	printf("** pam_sm_acct_mgmt\n");
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh, int flags, 
		int argc, const char **argv) {
	printf("** pam_sm_authenticate\n");

	const char* username;
	int retval = pam_get_user(pamh, &username, "Username: ");

	if (retval != PAM_SUCCESS) {
		printf("** pam_get_user is NOT okay!\n");
		return retval;
	}

	const char* password;
	retval = pam_get_item(pamh, PAM_AUTHTOK, (void*) &password);
	if (retval != PAM_SUCCESS) {
		printf("** PAM_AUTHTOK is NOT okay!\n");
		return retval;
	}

	printf("** %s\n", password);

	if (strcmp(username, TEST_USER) != 0) {
		printf("** You're not even my real admin!\n");
		return PAM_AUTH_ERR;
	}

	printf("** YOU LOOK VERY NICE TODAY<EOF>\n");
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_open_session(pam_handle_t *pamh, int flags, 
		int argc, const char **argv) {
	printf("** pam_sm_open_session\n");
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_close_session(pam_handle_t *pamh, int flags, 
		int argc, const char **argv) {
	printf("** pam_sm_close_session\n");
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_chauthtok(pam_handle_t *pamh, int flags, 
		int argc, const char **argv) {
	printf("** pam_sm_chauthtok\n");
	return PAM_SUCCESS;
}
