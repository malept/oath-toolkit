local-checks-to-skip = sc_GPL_version sc_immutable_NEWS sc_prohibit_strcmp

# Re-add when we have translation
local-checks-to-skip += sc_unmarked_diagnostics

.PHONY: ChangeLog
ChangeLog:
	git2cl > ChangeLog
	cat .clcopying >> ChangeLog
