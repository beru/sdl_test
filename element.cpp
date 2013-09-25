#include "element.h"

Element::Element()
{
	pos.x = pos.y = pivot.x = pivot.y = 0.0f;
	scale.x = scale.y = 1.0f;
	parent = -1;
}

ElementList::ElementList(Element** storage, uint16_t capacity)
	:
	storage(storage),
	capacity(capacity)
{
	
}

bool ElementList::Add(Element* e)
{
	for (size_t i=0; i<capacity; ++i) {
		if (!storage[i]) {
			storage[i] = e;
			e->id = i;
			return true;
		}
	}
	return false;
}

bool ElementList::Remove(Element* e)
{
	for (size_t i=0; i<capacity; ++i) {
		if (storage[i] == e) {
			storage[i] = 0;
			e->id = 0;
			return true;
		}
	}
	return false;
}

Element* ElementList::GetParent(Element* e)
{
	return storage[ e->parent ];
}

void ElementList::SetParent(Element* e, const Element* parent)
{
	e->parent = parent->id;
}

size_t ElementList::GetChildren(const Element* parent, Element** elements)
{
	size_t count = 0;
	uint16_t parentId = parent->id;
	for (size_t i=0; i<capacity; ++i) {
		Element* e = storage[i];
		if (!e) {
			continue;
		}
		if (e->parent == parentId) {
			elements[count++] = e;
		}
	}
	return count;
}

void ElementList::Update()
{
	for (size_t i=0; i<capacity; ++i) {
		Element* e = storage[i];
		if (!e) {
			continue;
		}
		e->Update();
	}
}

void ElementList::UpdateMatrix(Element* e, Matrix mat)
{
	Matrix m = mat;
	m *= Matrix::Translation(e->pos.x, e->pos.y);
	if (e->angle) {
		m *= Matrix::Translation(e->pivot.x, e->pivot.y);
		m *= Matrix::Rotation(e->angle);
	}
	m *= Matrix::Scaling(e->scale.x, e->scale.y);
	if (e->angle) {
		m *= Matrix::Translation(-e->pivot.x, -e->pivot.y);
	}
	e->mat = m;
	Element* children[128];
	size_t count = GetChildren(e, children);
	for (size_t i=0; i<count; ++i) {
		UpdateMatrix(children[i], m);
	}
}

void ElementList::Render()
{
	for (size_t i=0; i<capacity; ++i) {
		Element* e = storage[i];
		if (!e) {
			continue;
		}
		e->Render();
	}
}

size_t ElementList::HitTest(Point pt, Element** elements)
{
	size_t count = 0;
	for (size_t i=0; i<capacity; ++i) {
		Element* e = storage[i];
		if (!e) {
			continue;
		}
		if (e->HitTest(pt)) {
			elements[count++] = e;
		}
	}
	return count;
}

size_t ElementList::HitTest(Element* checkElement, Element** elements)
{
	size_t count = 0;
	for (size_t i=0; i<capacity; ++i) {
		Element* e = storage[i];
		if (!e || e == checkElement) {
			continue;
		}
		if (e->HitTest(checkElement)) {
			elements[count++] = e;
		}
	}
	return count;
}

