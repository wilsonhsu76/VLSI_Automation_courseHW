#include <iostream>
#include <cmath>
#include <float.h>
using namespace std;

#define GRADIENT_STOP 0.000001
#define SEARCH_TH_Y   0.00000000000001
#define SEARCH_TH_X   0.00000000000001
#define STEP_TH       10

struct search_range{
	double left;
	double center;
	double right;
};

struct x_vector {
	double x1;
	double x2;
};

//input: x = <x1,x2>
//output: return function value, f(x)
double q_fun(x_vector x) {
	double ret = 0.0;
	double x1 = x.x1;
	double x2 = x.x2;
	ret = x1*(x1 - 13) + x2*x2 + x1*(x2 + 7); //f(x1,x2)
	return ret;
}

//input: x = <x1,x2>
//output: return function gradient at x
//its unit length is 1
x_vector q_fun_gradient(x_vector x) {
	x_vector grad = { grad.x1 = 0.0, grad.x2 = 0.0 };
	double x1 = x.x1;
	double x2 = x.x2;
	grad.x1 = x1 * 2 + x2 - 6; //gradient of f(x1,x2)
	grad.x2 = x2 * 2 + x1;
	return grad;
}

double get_gradient_value(x_vector x) {
	double ret = 0.0;
	double x1 = x.x1;
	double x2 = x.x2;
	ret = sqrt(x1*x1 + x2*x2);
	return ret;
}

x_vector x_add_step_size(x_vector x, x_vector grad_x, double step_size) {
	x_vector ret_x = { ret_x.x1 = 0.0, ret_x.x2 = 0.0 };
	ret_x.x1 = x.x1 + step_size*grad_x.x1;
	ret_x.x2 = x.x2 + step_size*grad_x.x2;
	return ret_x;
}

//return step size: 0 to 10
//input grad_x: unit vector(direction), x: start point for this test
double decide_step_size(x_vector grad_x, x_vector x) {
	double ret = 0.0;
	int smallest_index = 0;
	double smallest_value = DBL_MAX;
	x_vector test_x = { test_x.x1 = 0.0, test_x.x2 = 0.0 };
	for (int i = 0; i < STEP_TH+1; i++) {
		test_x = x_add_step_size(x, grad_x, (double)i);
		double fun_value = q_fun(test_x);
		if (fun_value <= smallest_value) {
			smallest_value = fun_value;
			smallest_index = i;
		}
	}
	if (smallest_index == STEP_TH){
		ret = STEP_TH;
		return ret;
	}

	search_range search_obj = { search_obj.left = 0.0, search_obj.center = 0.0, search_obj.right = 0.0 };
	if (smallest_index != 0) {
		search_obj.left = smallest_index - 1;
		search_obj.center = smallest_index;
		search_obj.right = smallest_index + 1;
	}
	else {  //samller case
		search_obj.left = 0.0;
		search_obj.center = 0.5;
		search_obj.right = 1.0;
	}

	double left_val = q_fun(x_add_step_size(x, grad_x, search_obj.left));
	double center_val = q_fun(x_add_step_size(x, grad_x, search_obj.center));
	double right_val = q_fun(x_add_step_size(x, grad_x, search_obj.right));
	//reduce search size half per loop
	while( ((abs(center_val - left_val) > SEARCH_TH_Y) || (abs(center_val - right_val) > SEARCH_TH_Y)) &&
		   ((abs(search_obj.center - search_obj.left) > SEARCH_TH_X) || (abs(search_obj.right - search_obj.center) > SEARCH_TH_X)) ){
		double try_step = (search_obj.left + search_obj.center) / 2.0;  //left half
		test_x = x_add_step_size(x, grad_x, try_step);
		double try_val = q_fun(test_x);
		if (try_val < center_val) {
			search_obj.center = try_step;
			search_obj.right = search_obj.center;
			center_val = q_fun(x_add_step_size(x, grad_x, search_obj.center));
			right_val = q_fun(x_add_step_size(x, grad_x, search_obj.right));
			ret = search_obj.center;
			continue;
		}
		search_obj.left = try_step;
		try_step = (search_obj.right + search_obj.center) / 2.0;   //right half
		test_x = x_add_step_size(x, grad_x, try_step);
		try_val = q_fun(test_x);
		if (try_val > center_val) {
			search_obj.right = try_step;
			left_val = q_fun(x_add_step_size(x, grad_x, search_obj.left));
			right_val = q_fun(x_add_step_size(x, grad_x, search_obj.right));
		}
		else {
			search_obj.left = search_obj.center;
			search_obj.center = try_step;
			left_val = q_fun(x_add_step_size(x, grad_x, search_obj.left));
			center_val = q_fun(x_add_step_size(x, grad_x, search_obj.center));
		}
		ret = search_obj.center;
	}
	return ret;
}

x_vector Steepest_Descent_Heuristic() {
	x_vector x = { x.x1 = 0.0, x.x2 = 0.0 }; //test point
	x_vector next_x = { next_x.x1 = 0.0, next_x.x2 = 0.0 };
	x_vector x_grad = q_fun_gradient(x);
	double grad_value = get_gradient_value(x_grad);
	int tt = 0;
	while (grad_value >= GRADIENT_STOP) {
		x_vector x_unit_grad = { x_unit_grad.x1 = 0.0, x_unit_grad.x2 = 0.0 };
		x_unit_grad.x1 = -(x_grad.x1 / grad_value);
		x_unit_grad.x2 = -(x_grad.x2 / grad_value); //- for minimum direction
		double step_size = decide_step_size(x_unit_grad, x);
		next_x.x1 = x.x1 + step_size*(x_unit_grad.x1);
		next_x.x2 = x.x2 + step_size*(x_unit_grad.x2);

		//for next loop
		x = next_x;
		x_grad = q_fun_gradient(x);
		grad_value = get_gradient_value(x_grad);
		tt++;
		cout << "local minimum is at < " << x.x1 << ", " << x.x2 << ">,";
		cout << "  norm (gradient):" << grad_value << endl;
	}
	return x; //x is acceptable optimal solution point
}


int main() {
	cout.precision(8);
	x_vector res = Steepest_Descent_Heuristic();
	cout << "[result]: local minimum is at < " << res.x1 << ", " << res.x2 << ">,";
	cout << " f(x):" << q_fun(res) << endl;
	cin.get();
	return 0;
}