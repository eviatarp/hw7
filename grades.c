#include "grades.h"
#include "linked-list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {error = -1};

/* define struct course with name and grade	*/
struct course {
	char *name;
	int grade;	
};

/* define struct student with id, name, sum of grades, and a list of courses */
struct student {
	struct list *course_list;
    char *name;
    double sum_of_grades;
	int id;
};

/* define the complete linked list of grades */
struct grades{
	struct list *student_list;
};

/**
 * @brief clones a student element
 * @param student_in is a pointer to element
 * @param student_out is a pointer to pointer to element 
 * @returns 0 on success or 1 on fail
 * @note Failes if "student_in" or "student_out" is invalid
 */
static int student_clone (void *student_in, void **student_out) {   
	if(!student_in) {
		return 1;
	}
	struct student *student = (struct student*)student_in;
	struct student **out = (struct student**)student_out;
	struct student *copy = (struct student*)malloc(sizeof(*student));
	if(copy == NULL) {
		return 1;
	}
	*copy = *student;
	int name_size = strlen(student->name);
	char *temp_name = malloc(name_size+1);
	if(temp_name == NULL) {
		free(copy);
		return 1;
	}
	copy->name = temp_name;
	strcpy(temp_name, student->name);
	*out = copy;
	return 0;
};

/**
 * @brief destroy a student element
 * @param student_in is a pointer to element
 */
static void student_destroy (void *student_in) {  
	if(!student_in) {
		return;
	}
	struct student *student = (struct student*)student_in;
	list_destroy(student->course_list); 
	free(student->name);
	free (student_in);
};

/**
 * @brief clones a course element
 * @param course_in is a pointer to element
 * @param course_out is a pointer to pointer to element 
 * @returns 0 on success or 1 on fail
 * @note Failes if "course_in" or "course_out" is invalid
 */
static int course_clone (void *course_in, void **course_out) {    
	if(!course_in) {
		return 1;
	}
	struct course *course = (struct course*)course_in;
	struct course **out = (struct course**)course_out;
	struct course *copy = (struct course*)malloc(sizeof(*course));
	if(copy == NULL) {
		return 1;
	}
	*copy = *course;
	int name_size = strlen(course->name);
	char *temp_name = malloc(name_size+1);
	if(temp_name == NULL) {
		free(copy);
		return 1;
	}
	copy->name = temp_name;
	strcpy(temp_name, course->name);
	*out = copy;
	return 0;
};

/**  
 * @brief destroy a course element
 * @param course_in is a pointer to element
 */
static void course_destroy (void *course_in) {                     
	if(!course_in) {
		return;
	}
	struct course *course = (struct course*)course_in;
	free(course->name);
	free(course_in);
};

struct grades* grades_init() {
	struct list *student_list = list_init(&student_clone,&student_destroy); 
	if(student_list == NULL) {
		return NULL;		
	}
	struct grades *g = (struct grades*)malloc(sizeof(struct grades)); 
	if(g == NULL) {
		list_destroy(student_list);	
		return NULL;
	}
	g->student_list = student_list; 
	return g;
};

void grades_destroy(struct grades *grades){
	if(!grades) {
		return;
	}
	list_destroy(grades->student_list); 
	free(grades); 
};

int grades_add_student(struct grades *grades, const char *name, int id){  
	if(!grades) {
		return 1;
	}
	struct node *it = list_begin(grades->student_list);
	while(it) {
		if (((struct student*)list_get(it))->id == id){  
			return 1;
		}
		it = list_next(it);
	}
	struct student new_student;
	new_student.course_list = list_init(&course_clone,&course_destroy);
    if(new_student.course_list == NULL) {
    	list_destroy(new_student.course_list);
		return 1;		
	}
	new_student.name = (char*)name;
	new_student.sum_of_grades = 0;
	new_student.id = id;
	if(list_push_back(grades->student_list, &new_student) != 0) { 
		list_destroy(new_student.course_list);
		return 1;
	}
	return 0;
}; 

int grades_add_grade(struct grades *grades,
                     const char *name,
                     int id,
                     int grade) {                                      
	if(grade > 100 || grade < 0) {
		return 1;
	}
	struct node *it_student = list_begin(grades->student_list);	
	while(it_student) {
		if (((struct student*)list_get(it_student))->id == id) {
			struct student *cur_student = (struct student*)list_get(it_student);
			struct node *it_course = list_begin(cur_student->course_list);
			while(it_course) {
				if(!strcmp(((struct course*)list_get(it_course))->name, name)) {
					return 1;
				}
				it_course = list_next(it_course);
			}
			struct course new_course;
			new_course.name = (char*)name; 
			new_course.grade = grade;
			if(list_push_back(cur_student->course_list, &new_course) != 0) { 
				return 1;
			}
			cur_student->sum_of_grades = cur_student->sum_of_grades+grade;
			return 0;
		}
		it_student = list_next(it_student);
	}
	return 1;			 
};

float grades_calc_avg(struct grades *grades, int id, char **out) {
	if(grades == NULL) {
		*out = NULL;
		return error;
	}
	struct node *it_student = list_begin(grades->student_list);	
	while(it_student) {
		if (((struct student*)list_get(it_student))->id == id) {
			struct student *cur_student = (struct student*)list_get(it_student);
			double num_of_courses = (double)list_size(cur_student->course_list);
			int name_size = strlen(cur_student->name);
			*out = malloc(name_size+1);
			if(out == NULL) {
				return 1;
			}
			strcpy(*out,cur_student->name);
			if(num_of_courses == 0) {
				return 0;
			}
			float avg = (float)(cur_student->sum_of_grades)/(num_of_courses);
			return avg;
		}
		it_student = list_next(it_student);	
	}
	*out = NULL;
	return error;
}

int grades_print_student(struct grades *grades, int id) {
	if(grades == NULL) {
		return 1;
	}
	struct node *it_student = list_begin(grades->student_list);	
	while(it_student) {
		if (((struct student*)list_get(it_student))->id == id) { 
			struct student *cur_student = (struct student*)list_get(it_student);
			printf("%s %d:", cur_student->name, id);
			struct node *it_course = list_begin(cur_student->course_list);
			if(it_course) {
				struct course *tmp_course = (struct course*)list_get(it_course);
				printf(" %s %d", tmp_course->name, tmp_course->grade);
				it_course = list_next(it_course);
			}
			while(it_course) {
				struct course *tmp_course = (struct course*)list_get(it_course);
				printf(", %s %d", tmp_course->name, tmp_course->grade);
				it_course = list_next(it_course);
			}	
			printf("\n");
			return 0;
		}
	it_student = list_next(it_student);
	}
	return 1;
}


int grades_print_all(struct grades *grades) {
	if(grades == NULL) {
		return 1;
	}
	struct node *it_student = list_begin(grades->student_list);	
	while(it_student) {
		struct student *cur_student = (struct student*)list_get(it_student);
		printf("%s %d:", cur_student->name, cur_student->id);
		struct node *it_course = list_begin(cur_student->course_list);
		if(it_course) {
			struct course *temp_course = (struct course*)list_get(it_course);
			printf(" %s %d", temp_course->name, temp_course->grade);
			it_course = list_next(it_course);
		}
		while(it_course) {
			struct course *temp_course = (struct course*)list_get(it_course);
			printf(", %s %d", temp_course->name, temp_course->grade);
			it_course = list_next(it_course);
		}	
		printf("\n");
		it_student = list_next(it_student);
	}
	return 0;
}