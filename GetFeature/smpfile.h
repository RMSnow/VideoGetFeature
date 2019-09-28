#pragma once

class smp {
public:
	int index; //freame index
	double rgb_grade; 
	int cluster_id;

public:
	void setIndex(const int& index) { this->index = index; };
	void setGrade(const double& grade) { this->rgb_grade = grade; };
	void setCluster(const int& cluster_id) { this->cluster_id = cluster_id; };
};