#pragma once

#include <ClientBase/IView.h>

#include <list>
#include <memory>
#include <string>

namespace CoherentUIOpenGLUI {
	class ViewEventListener;

	class View final : public GUI::Elements::IView {
		std::wstring m_Path;

	public:
		View(std::wstring path);
		const std::wstring& GetPath();
	};

	class ViewGenerator final : public GUI::Elements::IViewGenerator {
		ViewEventListener &m_ViewListener;
		std::list<std::unique_ptr<GUI::Elements::IElementGenerator>> m_ElementGenerators;

	public:
		ViewGenerator(ViewEventListener &viewListener);

	public: // Generation
		std::unique_ptr<GUI::Elements::IView> GenerateView();
		void Generate(std::ostream &output);

	private: // Internal Generation Helpers
		void GenerateBody(std::ostream &output);

	public: // Adding Subcomponents
		void Add(std::unique_ptr<GUI::Elements::IElementGenerator> element);
	};
}