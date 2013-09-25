#pragma once

#include <stdint.h>
#include "matrix.h"

struct Element
{
public:
	Element();
	virtual ~Element() {}
	
	Point pos;
	Point pivot;
	Point scale;
	float angle;
	
protected:
	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual bool HitTest(Point pt) = 0;
	virtual bool HitTest(Element* e) = 0;
	
	friend class ElementList;
	ElementList* list;
	uint16_t id;
	uint16_t parent;
	Matrix mat;
};

class ElementList
{
public:
	ElementList(Element** storage, uint16_t capacity);
	bool Add(Element* e);
	bool Remove(Element* e);
	
	Element* GetParent(Element* e);
	void SetParent(Element* e, const Element* parent);
	size_t GetChildren(const Element* parent, Element** elements);
	
	void UpdateMatrix(Element* e, Matrix mat);
	void Update();
	void Render();
	size_t HitTest(Point pt, Element** elements);
	size_t HitTest(Element* checkElement, Element** elements);
	
private:
	Element** storage;
	uint16_t capacity;
};

