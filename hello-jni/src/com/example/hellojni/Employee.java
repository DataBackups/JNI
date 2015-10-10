package com.example.hellojni;

public class Employee {
	private String name;
	private int age;
	
    //构造函数，什么都不做  
    public Employee(){ }  
      
    public Employee(int age ,String name){  
        this.age = age ;  
        this.name = name ;  
    } 
    
    public int getAge() {  
        return age;  
    }  
    public void setAge(int age) {  
        this.age = age;  
    }  
    public String getName() {  
        return name;  
    }  
    public void setName(String name){  
        this.name = name;  
    } 
}
