$(exe): code $(all_source) $(all_headers) | $(output_dir)
	@echo "Making the executable..."
	@meta/exe.sh

$(package_archive): $(exe) | $(package_dir)
	echo "Making '$(package_archive)'..."
	@meta/package_archive.sh

$(output_dir): | $(platform_dir)
	mkdir $(output_dir)

$(package_dir): | $(platform_dir)
	mkdir $(package_dir)

$(platform_dir): | $(base_dir)
	mkdir $(platform_dir)

$(base_dir):
	mkdir $(base_dir)
