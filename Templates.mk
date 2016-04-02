
# $1: Project Name
# $2: Output Filename
# $3: Source Directory
# $4: Additonal CPPFLAGS
# $5: Additional LDFLAGS
# $6: Additional LDLIBS
# $7: Pre-requisites
define LibraryTemplate

$(1)_SRC      = $$(wildcard $(3)/*.cpp)
$(1)_OBJ      = $$(addprefix $$(OBJ_DIR)/, $$($(1)_SRC:.cpp=.o))
$(1)_OBJ_TEST = $$(addprefix $$(OBJ_DIR_TEST)/, $$($(1)_SRC:.cpp=.o))

$(1)_OUT      = $$(OUT_DIR)/$(2)
$(1)_OUT_TEST = $$(OUT_DIR)/Test$(1)

$$($(1)_OUT): $$($(1)_OBJ)

$$($(1)_OUT_TEST): $$($(1)_OBJ_TEST)
	$$(LD) $$(LDFLAGS) -o $$@ $$($(1)_OBJ_TEST) $$(LDLIBS)

$(1): CPPFLAGS += $(4)
$(1): LDFLAGS  += $(5)
$(1): LDLIBS   += $(6)
$(1): $(7) $$($(1)_OUT)

test-$(1): CPPFLAGS += $(4) $(TEST_CPPFLAGS)
test-$(1): LDFLAGS  += $(5) $(TEST_LDFLAGS)
test-$(1): LDLIBS   += $(6) $(TEST_LDLIBS)
test-$(1): $$($(1)_OUT_TEST) $(7)
	cd $(1); ../$$($(1)_OUT_TEST)

clean-$(1):
	rm -f $$($(1)_OUT) $$($(1)_OUT_TEST) $$($(1)_OBJ) $$($(1)_OBJ_TEST)

endef

# $1: Project Name
# $2: Output Filename
# $3: Source Directory
# $4: Additonal CPPFLAGS
# $5: Additional LDFLAGS
# $6: Additional LDLIBS
# $7: Pre-requisites
# $8: Command-line Arguments
define ExecutableTemplate

$(1)_SRC      = $$(wildcard $(3)/*.cpp)
$(1)_OBJ      = $$(addprefix $$(OBJ_DIR)/, $$($(1)_SRC:.cpp=.o))
$(1)_OBJ_TEST = $$(addprefix $$(OBJ_DIR_TEST)/, $$($(1)_SRC:.cpp=.o))

$(1)_OUT      = $$(OUT_DIR)/$(2)
$(1)_OUT_TEST = $$(OUT_DIR)/Test$(1)

$$($(1)_OUT): $$($(1)_OBJ) $(7)
	$$(LD) $$(LDFLAGS) -o $$@ $$($(1)_OBJ) $$(LDLIBS)

$$($(1)_OUT_TEST): $$($(1)_OBJ_TEST) $(7)
	$$(LD) $$(LDFLAGS) -o $$@ $$($(1)_OBJ_TEST) $$(LDLIBS)

$(1): CPPFLAGS += $(4)
$(1): LDFLAGS  += $(5)
$(1): LDLIBS   += $(6)
$(1): $$($(1)_OUT)

test-$(1): CPPFLAGS += $(4) $(TEST_CPPFLAGS)
test-$(1): LDFLAGS  += $(5) $(TEST_LDFLAGS)
test-$(1): LDLIBS   += $(6) $(TEST_LDLIBS)
test-$(1): $$($(1)_OUT_TEST)
	cd $(1); ../$$($(1)_OUT_TEST)

clean-$(1):
	rm -f $$($(1)_OUT) $$($(1)_OUT_TEST) $$($(1)_OBJ) $$($(1)_OBJ_TEST)

run-$(1):
	cd $(1); ../$$($(1)_OUT) $(8)

debug-$(1):
	cd $(1); gdb --args ../$$($(1)_OUT) $(8)

profile-$(1):
	cd $(1); valgrind ../$$($(1)_OUT) --args $(8)

endef
