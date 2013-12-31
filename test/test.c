#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <json/json.h>
#include <json/json_tokener.h>

#define KEYSTONE_URL "http://keystone.stack.local:5000/v2.0/tokens"
#define KEYSTONE_TENANT "mytenant"
#define KEYSTONE_USER "myuser"
#define KEYSTONE_PASS "secret"

struct stringbuffer {
	char *ptr;
	size_t len;
};

void stringbuffer_init(struct stringbuffer* s) {
	s->len = 0;
	s->ptr = malloc(s->len + 1);
	if (s->ptr == NULL) {
		printf("malloc failed\n");
		exit(EXIT_FAILURE);
	}

	s->ptr[0] = '\0';
}

void stringbuffer_free(struct stringbuffer* s) {
	free(s->ptr);
	s->len = 0;
}

size_t http_received_data(void *ptr, size_t size, size_t nmemb, struct stringbuffer *s) {
	size_t new_len = s->len + (size * nmemb);
	s->ptr = realloc(s->ptr, new_len + 1);

	if (s->ptr == NULL) {
		fprintf(stderr, "realloc failed\n");
		exit(EXIT_FAILURE);
	}

	memcpy(s->ptr + s->len, ptr, size * nmemb);
	s->ptr[new_len] = '\0';
	s->len = new_len;

	return size * nmemb;
}

int main(int argc, char **argv) {
	CURL *curl;
	CURLcode res;
	struct curl_slist* slist = NULL;
	struct stringbuffer buffer;
	long resp_code = 0;
	char *token = NULL;

	curl = curl_easy_init();
	stringbuffer_init(&buffer);

	if (curl) {
		/** Let it be known that I refuse to write manual, string-quoted JSON. */
		slist = curl_slist_append(slist, "Content-type: application/json");

		json_object *body_obj = json_object_new_object();
		json_object *auth_obj = json_object_new_object();
		json_object *cred_obj = json_object_new_object();
		json_object *tenant_str = json_object_new_string(KEYSTONE_TENANT);
		json_object *user_str = json_object_new_string(KEYSTONE_USER);
		json_object *pass_str = json_object_new_string(KEYSTONE_PASS);

		json_object_object_add(body_obj, "auth", auth_obj);
		json_object_object_add(auth_obj, "passwordCredentials", cred_obj);
		json_object_object_add(auth_obj, "tenantName", tenant_str);
		json_object_object_add(cred_obj, "username", user_str);
		json_object_object_add(cred_obj, "password", pass_str);

		const char* body = json_object_to_json_string(body_obj);
		//printf("body[%d]:\n%s\n\n", strlen(body), body);

		curl_easy_setopt(curl, CURLOPT_URL, KEYSTONE_URL);
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, (void*) http_received_data);

		res = curl_easy_perform(curl);

		json_object_put(body_obj);
		json_object_put(auth_obj);
		json_object_put(cred_obj);
		json_object_put(user_str);
		json_object_put(pass_str);
		json_object_put(tenant_str);

		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &resp_code);

		//printf("HTTP %d\n", resp_code);
		//printf("body[%d]\n%s\n\n", buffer.len, buffer.ptr);

		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));
		}
		else if (resp_code == 200) {
			json_object *json = json_tokener_parse(buffer.ptr);
			json_object *access_obj = json_object_object_get(json, "access");
			json_object *token_obj = json_object_object_get(access_obj, "token");
			json_object *id_obj = json_object_object_get(token_obj, "id");

			const char* tmp_token = json_object_get_string(id_obj);
			token = malloc(strlen(tmp_token) + 1);
			strcpy(token, tmp_token);

			json_object_put(id_obj);
			json_object_put(token_obj);
			json_object_put(access_obj);
			json_object_put(json);
		}
		else {
			fprintf(stderr, "Could not authenticate (HTTP %d received).\n", resp_code);
		}

		stringbuffer_free(&buffer);
		curl_easy_cleanup(curl);
		curl_slist_free_all(slist);
	}

	if (token != NULL) {
		printf("token is %s\n", token);
	}

	return 0;
}
