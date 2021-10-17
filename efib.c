/*
    efib looks for file signatures in binary files and extracts their files
    Copyright (C) 2021 happysmash27

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>

typedef struct file_se_sig {
     const char* extension;
     const unsigned char* start_sig;
     const size_t start_sig_end;
     const unsigned char* end_sig;
     const size_t end_sig_end;
} file_se_sig;

//TODO: Could potentially turn this into something based off a radix tree (https://en.wikipedia.org/wiki/Radix_tree) in the future
const unsigned char png_start_sig[] = {0x89u, 0x50u, 0x4Eu, 0x47u, 0x0Du, 0x0Au, 0x1Au, 0x0Au};
const unsigned char png_end_sig[] = {0x49u, 0x45u, 0x4Eu, 0x44u, 0xAEu, 0x42u, 0x60u, 0x82u};
const file_se_sig file_sigs[] = {
     {
	  .extension = "png",
	  .start_sig = png_start_sig,
	  .start_sig_end = sizeof(png_start_sig)-1,
	  .end_sig = png_end_sig,
	  .end_sig_end = sizeof(png_end_sig)-1
     }
};
const size_t num_file_sigs = sizeof(file_sigs)/sizeof(struct file_se_sig);

void print_signature(FILE* outfile, const unsigned char* signature, const size_t end_index){
     for (size_t i=0; i<=end_index; i++){
	  fputc(signature[i], outfile);
     }
}

int main(){
     //Input Character
     //int because that's what getc gives
     int inc;

     char in_file = 0;
     FILE* write_file;
     size_t write_sig_index;
     size_t sig_loc_index[num_file_sigs];
     size_t end_sig_loc = 0;
     //TODO: Make this still work when multiple file signatures share the same extension
     size_t files_written[num_file_sigs];
     //Initialise all arrays of size num_file_sigs to 0
     for (size_t i = 0; i<num_file_sigs; i++){
	  sig_loc_index[i] = 0;
	  files_written[i] = 0;
     }
     while ((inc = getc(stdin)) != EOF){
	  //Could make this support files in files
	  //But it seems simpler and less bug-prone to just ignore indicators if we are already in a file
	  if (!in_file){
	       //Check for matches in every file signature
	       for (size_t i=0; i<num_file_sigs; i++){
		    //If our input character matches the next character in the file signature
		    //Continue
		    //Otherwise, reset the counter
		    if (inc == file_sigs[i].start_sig[sig_loc_index[i]]){
			 if (sig_loc_index[i] == file_sigs[i].start_sig_end){
			      in_file = 1;
			      write_sig_index = i;
			      //Generate a file name
			      int needed_name_size = snprintf(NULL, 0, "%lu.%s",
							      files_written[write_sig_index],
							      file_sigs[write_sig_index].extension);
			      char filename[needed_name_size];
			      snprintf(filename, needed_name_size+1, "%lu.%s",
				       files_written[write_sig_index],
				       file_sigs[write_sig_index].extension);
			      //Open our file
			      write_file = fopen(filename, "w");
			      if (write_file == NULL){
				   perror("Couldn't open file");
				   exit(EXIT_FAILURE);
			      }
			      //Print file signature we just checked exists but did not print
			      print_signature(write_file,
					      file_sigs[write_sig_index].start_sig,
					      file_sigs[write_sig_index].start_sig_end);
			      sig_loc_index[i] = 0;
			 } else {
			      sig_loc_index[i]++;
			 }
		    } else {
			 sig_loc_index[i] = 0;
		    }
	       }
	  } else {
	       //What to do while we are in a file

	       //Print character
	       fputc(inc, write_file);
	       
	       //Check for matches in our end signature
	       //If our input character matches the next character in the file end signature
	       //Up the counter, or if we are at the end, go out of our in_file mode, reset and close things
	       //Otherwise, just reset the file end signature location
	       if (inc == file_sigs[write_sig_index].end_sig[end_sig_loc]){
		    if (end_sig_loc == file_sigs[write_sig_index].end_sig_end){
			 //Reset and close things
			 if(fclose(write_file)){
			      perror("Failed to close output file");
			      exit(EXIT_FAILURE);
			 }
			 in_file = 0;
			 end_sig_loc = 0;
		    } else {
			 end_sig_loc++;
		    }
	       } else {
		    end_sig_loc = 0;
	       }
	  }
     }

     exit(EXIT_SUCCESS);
}
