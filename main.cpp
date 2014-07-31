// Ŭnicode please

#include <cstdio>
#include <cassert>
#include <type_traits>
#include "bitfield_dynamic_cast.h"

#include <ctime>	// for benchmark
#include "loki/Typelist.h"

// Bit Mask for Interface
enum {
	kFoo = 1 << 0,
	kBar = 1 << 1,
	kSpam = 1 << 2,
};

// Define Interface
struct IFoo : public sora::IBaseInterface < kFoo > {
	virtual ~IFoo() = 0 {}
	virtual void Foo() = 0;
	virtual void FooBenchmark () = 0;
};

struct IBar : public sora::IBaseInterface < kBar > {
	virtual ~IBar() = 0 {}
	virtual void Bar() = 0;
	virtual void BarBenchmark() = 0;
};

struct ISpam : public sora::IBaseInterface < kSpam > {
	virtual ~ISpam() = 0 {}
	virtual void Spam() = 0;
	virtual void SpamBenchmark() = 0;
};

// Define Class
class FooBar : public sora::BitFieldInheritance<TYPELIST_2(IFoo, IBar)> {
public:
	FooBar() : counter(0) {}
	virtual ~FooBar() {}
	virtual void Foo() { printf("A - Foo\n"); }
	virtual void Bar() { printf("A - Bar\n"); }

	virtual void FooBenchmark() { counter++; }
	virtual void BarBenchmark() { counter++; }

private:
	int counter;
};

class BarSpam : public sora::BitFieldInheritance<TYPELIST_2(IBar, ISpam)> {
public:
	BarSpam() : counter(0) {}
	virtual ~BarSpam() {}
	virtual void Bar() { printf("B - Bar\n"); }
	virtual void Spam() { printf("B - Spam\n"); }

	virtual void BarBenchmark() { counter++; }
	virtual void SpamBenchmark() { counter++; }
private:
	int counter;
};


// compile-time inheritance validation
static_assert(std::is_base_of<IFoo, FooBar>::value == 1, "");
static_assert(std::is_base_of<IBar, FooBar>::value == 1, "");
static_assert(std::is_base_of<ISpam, FooBar>::value == 0, "");

static_assert(std::is_base_of<IFoo, BarSpam>::value == 0, "");
static_assert(std::is_base_of<IBar, BarSpam>::value == 1, "");
static_assert(std::is_base_of<ISpam, BarSpam>::value == 1, "");

void validate_runtime_inheritance()
{
	sora::IObject *a = new FooBar();
	sora::IObject *b = new BarSpam();

	// run-time inheritanct validation
	assert(IFoo::IsDerived(a) == true);
	assert(IBar::IsDerived(a) == true);
	assert(ISpam::IsDerived(a) == false);

	assert(IFoo::IsDerived(b) == false);
	assert(IBar::IsDerived(b) == true);
	assert(ISpam::IsDerived(b) == true);

	delete(a);
	delete(b);
}

void use_bitfield_dynamic_cast()
{
	sora::IObject *a = new FooBar();
	sora::IObject *b = new BarSpam();

	// bitfield_dynamic_cast
	IFoo *a_foo = sora::bitfield_dynamic_cast<IFoo*>(a);
	IBar *a_bar = sora::bitfield_dynamic_cast<IBar*>(a);
	ISpam *a_spam = sora::bitfield_dynamic_cast<ISpam*>(a);
	a_foo->Foo();
	a_bar->Bar();
	assert(a_spam == nullptr);

	IFoo *b_foo = sora::bitfield_dynamic_cast<IFoo*>(b);
	IBar *b_bar = sora::bitfield_dynamic_cast<IBar*>(b);
	ISpam *b_spam = sora::bitfield_dynamic_cast<ISpam*>(b);
	assert(b_foo == nullptr);
	b_bar->Bar();
	b_spam->Spam();

	delete(a);
	delete(b);
}

void benchmark_dynamic_cast(int loop)
{
	sora::IObject *obj = new FooBar();
	clock_t start = clock();

	// benchmark begin
	for (int i = 0; i < loop; ++i) {
		IFoo *foo = dynamic_cast<IFoo*>(obj);
		if (foo) {
			foo->FooBenchmark();
		}
		IBar *bar = dynamic_cast<IBar*>(obj);
		if (bar) {
			bar->BarBenchmark();
		}
		ISpam *spam = dynamic_cast<ISpam*>(obj);
		if (spam) {
			spam->SpamBenchmark();
		}
	}
	// benchmark end

	clock_t dt = clock() - start;
	printf("         dynamic_cast : %d = %f\n", loop, ((float)dt) / CLOCKS_PER_SEC);

	delete(obj);
}

void benchmark_bitfield_dynamic_cast(int loop)
{
	sora::IObject *obj = new FooBar();
	clock_t start = clock();

	// benchmark begin
	for (int i = 0; i < loop; ++i) {
		IFoo *foo = sora::bitfield_dynamic_cast<IFoo*>(obj);
		if (foo) {
			foo->FooBenchmark();
		}
		IBar *bar = sora::bitfield_dynamic_cast<IBar*>(obj);
		if (bar) {
			bar->BarBenchmark();
		}
		ISpam *spam = sora::bitfield_dynamic_cast<ISpam*>(obj);
		if (spam) {
			spam->SpamBenchmark();
		}
	}
	// benchmark end

	clock_t dt = clock() - start;
	printf("bitfield_dynamic_cast : %d = %f\n", loop, ((float)dt) / CLOCKS_PER_SEC);

	delete(obj);
}

int main()
{
	validate_runtime_inheritance();
	use_bitfield_dynamic_cast();

	int loop_array[] = {
		10000,
		100000,
		1000000,
		10000000,
		100000000,
	};
	int loop_array_size = sizeof(loop_array) / sizeof(loop_array[0]);
	for (int i = 0; i < loop_array_size; ++i) {
		int loop = loop_array[i];
		benchmark_dynamic_cast(loop);
		benchmark_bitfield_dynamic_cast(loop);
	}

	return 0;
}
