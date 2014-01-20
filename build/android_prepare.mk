ROCKET_LIB_NAME		:= rocket
ROCKET_LIB_VERSION	:= 0.0.1
ROCKET_LIB_ROOT		:= $(REPO_ROOT)/$(ROCKET_LIB_NAME)/$(ROCKET_LIB_VERSION)

ifeq ($(GAME_LABEL),)
GAME_LABEL=$(NAME)
endif

ifeq ($(GAME_PKGNAME),)
GAME_PKGNAME=$(ROCKET_LIB_NAME).$(NAME)
endif

prepare_android:
	mkdir -p "$(PROJECT_ROOT)/out"
	if [ ! -d "$(PROJECT_ROOT)/out/android" ]; then \
		cp -r $(ROCKET_LIB_ROOT)/build/android_proj "$(PROJECT_ROOT)/out/android"; \
		sh $(ROCKET_LIB_ROOT)/build/replace.sh "$(PROJECT_ROOT)/out/android/src/com/mridle/rocket2d/Rocket2dActivity.java" '###ROCKET_LIB###'  "$(NAME)"; \
		sh $(ROCKET_LIB_ROOT)/build/replace.sh "$(PROJECT_ROOT)/out/android/res/values/strings.xml" '###GAME_LABEL###'  "$(GAME_LABEL)"; \
		sh $(ROCKET_LIB_ROOT)/build/replace.sh "$(PROJECT_ROOT)/out/android/AndroidManifest.xml" '###GAME_PKGNAME###'  "$(GAME_PKGNAME)"; \
		if [ -d "$(PROJECT_ROOT)/assets" ]; then \
			ln -fs "$(PROJECT_ROOT)/assets" "$(PROJECT_ROOT)/out/android/assets"; \
		fi; \
	fi;
