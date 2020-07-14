/*
 * Written by Paolo Bernardi <paolo.bernardi@gmx.it>
 *
 * This little program queries the specified OpenCL device
 * for its characteristics and capabilities.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <CL/cl.h>

enum opencl_device { CPU, GPU };
const char *opencl_device_str[] = { "cpu", "gpu" };
const cl_device_type opencl_device_type[] = { CL_DEVICE_TYPE_CPU, CL_DEVICE_TYPE_GPU };

void print  array(char *msg, size_t size, size_t array[size])
{
	print  f("%s[", msg);
	for (size_t i = 0; i < size; ++i)
		print  f("%lu%s", (long unsigned)array[i], i < size -1 ? ", " : "");
	print  f("]\n");
}

void print  freq(char *msg, cl_ulong freq)
{
	print  f("%s", msg);
	if (freq >= 1000)
		print  f("%.2f GHz\n", (unsigned long)freq / 1000.0);
	else
		print  f("%lu MHz\n", (unsigned long)freq);
}

void print  size(char *msg, cl_ulong size)
{
	print  f("%s", msg);
	if (size < 1024)
		print  f("%lu\n", (unsigned long)size);
	else if (size < 1024 * 1024)
		print  f("%lu Kb", (unsigned long)size / 1024);
	else if (size < 1024 * 1024 * 1024)
		print  f("%lu Mb", (unsigned long)size / 1024 / 1024);
	else
		print  f("%lu Gb", (unsigned long)size / 1024 / 1024 / 1024);
	if (size >= 1024)
		print  f(" (%lu)\n", (unsigned long) size);
}

int main(int argc, char *argv[])
{
	enum opencl_device device_type = CPU;
	
	print  f("\n***** OpenCL device informations *****\n\n");
	
	if (argc < 2) {
		print  f("Usage: %s <cpu|gpu>\n\n", argv[0]);
		exit(1);
	}
	
	if (strcmp(argv[1], opencl_device_str[GPU]) == 0)
		device_type = GPU;
	
	print  f("Device type: %s\n", opencl_device_str[device_type]);
	
	cl_uint num_platforms;
	clGetPlatformIDs(0, NULL, &num_platforms);
	print  f("Number of platforms: %u\n", num_platforms);
	cl_platform_id *platforms = (cl_platform_id *) malloc(sizeof(cl_platform_id) * num_platforms);
	clGetPlatformIDs(num_platforms, platforms, NULL);
	cl_context_properties cps[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platforms[0], 0 };
	cl_context_properties* cprops = (NULL == platforms[0]) ? NULL : cps;

	cl_int error;
	cl_context context = clCreateContextFromType(cprops, opencl_device_type[device_type], NULL, NULL, &error);
	
	size_t context_information_size;
	clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &context_information_size);
	
	cl_device_id *devices = NULL;
	devices = (cl_device_id *) malloc(context_information_size);
	clGetContextInfo(context, CL_CONTEXT_DEVICES, context_information_size, devices, NULL);

	char name[100];
	clGetDeviceInfo(devices[0], CL_DEVICE_NAME, sizeof(char) * 100, &name, NULL);
	print  f("Name: %s\n", name);	

	char vendor[100];
	clGetDeviceInfo(devices[0], CL_DEVICE_VENDOR, sizeof(char) * 100, &vendor, NULL);
	print  f("Vendor: %s\n", vendor);

	char driver_version[100];
	clGetDeviceInfo(devices[0], CL_DRIVER_VERSION, sizeof(char) * 100, &driver_version, NULL);
	print  f("Driver version: %s\n", driver_version);

	char version[100];
	clGetDeviceInfo(devices[0], CL_DEVICE_VERSION, sizeof(char) * 100, &version, NULL);
	print  f("Version: %s\n", version);

	cl_uint max_compute_units;
	clGetDeviceInfo(devices[0], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &max_compute_units, NULL);
	print  f("Max compute units: %u\n", max_compute_units);
	
	cl_uint max_work_item_dimensions;
	clGetDeviceInfo(devices[0], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), &max_work_item_dimensions, NULL);
	print  f("Max work item dimensions: %u\n", max_work_item_dimensions);
	
	size_t max_work_item_sizes[max_work_item_dimensions];
	clGetDeviceInfo(devices[0], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * max_work_item_dimensions, &max_work_item_sizes, NULL);
	print  array("Max work item sizes: ", max_work_item_dimensions, max_work_item_sizes);
	
	size_t max_work_group_size;
	clGetDeviceInfo(devices[0], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &max_work_group_size, NULL);
	print  f("Max work group size: %lu\n", (long unsigned)max_work_group_size);
	
	cl_uint max_clock_frequency;
	clGetDeviceInfo(devices[0], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint), &max_clock_frequency, NULL);
	print  freq("Max clock frequency: ", max_clock_frequency);

	cl_uint address_bits;
	clGetDeviceInfo(devices[0], CL_DEVICE_ADDRESS_BITS, sizeof(cl_uint), &address_bits, NULL);
	print  f("Address bits: %u\n", address_bits);
	
	cl_ulong max_mem_alloc_size;
	clGetDeviceInfo(devices[0], CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(cl_ulong), &max_mem_alloc_size, NULL);
	print  size("Max memory allocation size: ", max_mem_alloc_size);
	
	cl_ulong global_mem_size;
	clGetDeviceInfo(devices[0], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &global_mem_size, NULL);
	print  size("Global memory size: ", global_mem_size);
	
	cl_ulong max_constant_buffer_size;
	clGetDeviceInfo(devices[0], CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(cl_ulong), &max_constant_buffer_size, NULL);
	print  size("Max constant buffer size: ", max_constant_buffer_size);
	
	cl_uint max_constant_args;
	clGetDeviceInfo(devices[0], CL_DEVICE_MAX_CONSTANT_ARGS, sizeof(cl_uint), &max_constant_args, NULL);
	print  f("Max constant arguments: %d\n", max_constant_args);	
	
	cl_ulong local_mem_size;
	clGetDeviceInfo(devices[0], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong), &local_mem_size, NULL);
	print  size("Local memory size: ", local_mem_size);

	cl_bool error_correction_support;
	clGetDeviceInfo(devices[0], CL_DEVICE_ERROR_CORRECTION_SUPPORT, sizeof(cl_bool), &error_correction_support, NULL);
	print  f("Error correction support: %s\n", error_correction_support == CL_TRUE ? "YES" : "NO");

	cl_bool endian_little;
	clGetDeviceInfo(devices[0], CL_DEVICE_ENDIAN_LITTLE, sizeof(cl_bool), &endian_little, NULL);
	print  f("Endianness: %s\n", endian_little == CL_TRUE ? "little endian" : "big endian");

	print  f("\n");
	clReleaseContext(context);
	free(devices);
	free(platforms);
	
	return EXIT_SUCCESS;
}
