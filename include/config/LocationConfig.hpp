/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmaeda <kmaeda@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 14:19:12 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/09 18:17:33 by kmaeda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>

class LocationConfig {
	private:
		std::string path;
		std::string cgiPath;
		std::string root;
		std::string indexFile;
		std::vector<std::string> allowedMethods;
		std::vector<std::string> cgiExtensions;
	
	public:
		LocationConfig();

		// Getters
		std::string getPath() const;
		std::string getCgiPath() const;
		std::vector<std::string> getAllowedMethods() const;
		std::vector<std::string> getCgiExtensions() const;
		std::string getRoot() const;
		std::string getIndexFile() const;

		// Setters
		void setPath(const std::string& path);
		void setCGIPath(const std::string& cgiPath);
		void addAllowedMethods(const std::string& method);
		void setCgiExtensions(const std::string& cgiExtension);
		void setRoot(const std::string& root);
		void setIndexFile(const std::string& index);

		bool isMethodAllowed(const std::string& method) const;
		void validate();
};

#endif
