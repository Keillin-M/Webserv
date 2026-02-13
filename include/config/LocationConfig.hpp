/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmaeda <kmaeda@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 14:19:12 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/13 12:17:51 by kmaeda           ###   ########.fr       */
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
		std::string CGIPath;
		std::string root;
		std::string indexFile;
		std::vector<std::string> allowedMethods;
		std::vector<std::string> CGIExtensions;
	
	public:
		LocationConfig();

		// Getters
		std::string getPath() const;
		std::vector<std::string> getAllowedMethods() const;
		std::string getRoot() const;
		std::string getIndexFile() const;
		std::vector<std::string> getCGIExtensions() const;
		std::string getCGIPath() const;

		// Setters
		void setPath(const std::string& path);
		void addAllowedMethods(const std::string& method);
		void setRoot(const std::string& root);
		void setIndexFile(const std::string& index);
		void setCGIExtensions(const std::string& CGIExtensions);
		void setCGIPath(const std::string& CGIPath);

		bool isMethodAllowed(const std::string& method) const;
		void validate();
};

#endif
