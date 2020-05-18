message("Update directory Src: ${src_dir} Dst: ${dst_dir}")

file(GLOB_RECURSE _file_list RELATIVE "${src_dir}" "${src_dir}/*")

foreach( each_file ${_file_list} )
	set(destinationfile "${dst_dir}/${each_file}")
	set(sourcefile "${src_dir}/${each_file}")
	if(NOT EXISTS ${destinationfile} OR ${sourcefile} IS_NEWER_THAN ${destinationfile})
		get_filename_component(destinationdir ${destinationfile} DIRECTORY)
		message("	Update Src: ${sourcefile} Dst: ${destinationfile}")
		file(COPY ${sourcefile} DESTINATION ${destinationdir})
	endif()
endforeach(each_file)
message("Update directory ended Src: ${src_dir} Dst: ${dst_dir}")