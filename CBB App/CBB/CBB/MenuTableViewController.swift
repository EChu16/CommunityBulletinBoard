//
//  MenuTableViewController.swift
//  CBB
//
//  Created by Erich Chu on 4/18/17.
//  Copyright © 2017 Erich Chu. All rights reserved.
//

import UIKit
import Alamofire
import SwiftyJSON

class MenuTableViewController: UIViewController, UITableViewDelegate, UITableViewDataSource {

    var menuNames:Array = [String]()
    var user_name = ""
    var user_id = ""
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Set Menu names here
        menuNames = ["padding", "Home", "Communities", "Settings", "Logout", "padding"]
        getUserInfo()
    }
    
    func getUserInfo() {
        // create post request
        let url = "https://code.engineering.nyu.edu:8080/cbb/home"
        Alamofire.request(url, method: .post, encoding: JSONEncoding.default).responseJSON { response in
            if (response.response?.statusCode == nil) {
                print("Couldn't connect to server.")
            }
            else if (response.response?.statusCode==400) {
                print ("hit")
                debugPrint(String(data: response.data!, encoding: String.Encoding.utf8))
                print(response)
            }
            else if (response.response?.statusCode==200) {
                guard let object = response.result.value else {
                    print("Error")
                    return
                }
                let json = JSON(object)
                if var info = json.array {
                    let user_info = info.removeLast()
                    self.user_name = user_info["username"].string!
                    self.user_id = (user_info["_id"].dictionary?["$oid"]?.string!)!
                }
            }
        }
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return menuNames.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "MenuTableViewCell") as! MenuTableViewCell
        
        cell.menuLabel.text! = menuNames[indexPath.row]
        
        if (indexPath.row == 0)  {
            cell.menuLabel.text! = ""
        }
        if (indexPath.row == menuNames.count - 1) {
            cell.menuLabel.text! = ""
        }
        
        cell.menuCellLine.backgroundColor = UIColor(colorLiteralRed: 179.0/255.0, green: 140.0/255.0, blue: 3.0/255.0, alpha: 1.0)
            
        return cell
    }
    
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        let revealViewController:SWRevealViewController = self.revealViewController()
        
        let cell:MenuTableViewCell = tableView.cellForRow(at: indexPath) as! MenuTableViewCell
        
        if (cell.menuLabel.text! == "Home") {
            let mainStoryboard:UIStoryboard = UIStoryboard(name: "Main", bundle: nil)
            let desController = mainStoryboard.instantiateViewController(withIdentifier: "UserBoardsViewController") as! UserBoardsViewController
            let newFrontViewController = UINavigationController.init(rootViewController:desController)
            revealViewController.pushFrontViewController(newFrontViewController, animated: true)
        }
        
        if (cell.menuLabel.text! == "Communities") {
            let mainStoryboard:UIStoryboard = UIStoryboard(name: "Main", bundle: nil)
            let desController = mainStoryboard.instantiateViewController(withIdentifier: "CommunityViewController") as! CommunityViewController
            desController.user_name = self.user_name
            desController.user_id = self.user_id
            let newFrontViewController = UINavigationController.init(rootViewController:desController)
            revealViewController.pushFrontViewController(newFrontViewController, animated: true)
        }
        
        if (cell.menuLabel.text! == "Settings") {
            let mainStoryboard:UIStoryboard = UIStoryboard(name: "Main", bundle: nil)
            let desController = mainStoryboard.instantiateViewController(withIdentifier: "SettingsViewController") as! SettingsViewController
            desController.user_name = self.user_name
            desController.user_id = self.user_id
            let newFrontViewController = UINavigationController.init(rootViewController:desController)
            revealViewController.pushFrontViewController(newFrontViewController, animated: true)
        }
        
        if (cell.menuLabel.text! == "Logout") {
            let mainStoryboard:UIStoryboard = UIStoryboard(name: "Main", bundle: nil)
            let desController = mainStoryboard.instantiateViewController(withIdentifier: "LoginViewController") as! LoginViewController
            revealViewController.pushFrontViewController(desController, animated: true)
        }
    }
}
